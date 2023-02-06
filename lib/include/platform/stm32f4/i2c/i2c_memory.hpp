#pragma once
#include "i2c_like.hpp"
#include "reg/field.hpp"
#include <array>
#include "async/receiver.hpp"
#include "async/future.hpp"
#include "async/use_state.hpp"
#include "async/and_then.hpp"
#include "async/map.hpp"
#include "async/sequence.hpp"

namespace drivers::i2c
{
    namespace detail
    {
        struct DefaultAddressSerializer
        {
            template<class T>
            static void serializeAddress(T t, std::uint8_t * buf)
            {
                *std::launder(reinterpret_cast<T *>(buf)) = t;
            }
        };

        template<class TAddress, class TValue>
        struct ReadRequest
        {
            TAddress address;
            TValue value;
        };
    }

    template<
        I2cLike I2cDevice,
        class RegTag, 
        class Serializer = detail::DefaultAddressSerializer>
    class I2cMemory
    {
        template<class TOffset, class TValue>
        struct TransferState
        {
            TransferState(TOffset offset)
            {
                Serializer::serializeAddress(offset, buffer);
            }

            TransferState(TOffset offset, TValue value)
            {
                Serializer::serializeAddress(offset, buffer);
                *getValuePtr() = value;
            }

            TValue * getValuePtr()
            {
                return std::launder(reinterpret_cast<TValue *>(buffer + sizeof(TOffset)));
            }

            TValue getValue()
            {
                return *getValuePtr();
            }

            std::uint8_t * addressBuffer()
            {
                return buffer;
            }

            std::uint8_t * valueBuffer()
            {
                return buffer+sizeof(TOffset);
            }

            constexpr std::uint16_t addressSize() const
            {
                return sizeof(TOffset);
            }

            constexpr std::uint16_t valueSize() const
            {
                return sizeof(TValue);
            }

            constexpr std::uint16_t bufferSize() const
            {
                return addressSize() + valueSize();
            }

            std::uint8_t buffer[sizeof(TOffset)+sizeof(TValue)];
        };

        template<class TOffset, class TValue, class F>
        struct ReadModifyWriteState : TransferState<TOffset, TValue>
        {
            template<class F2>
            ReadModifyWriteState(F2 && f, TOffset offset)
            : TransferState<TOffset, TValue>(offset)
            , transform(static_cast<F2&&>(f))
            {
                
            }

            template<class F2>
            ReadModifyWriteState(F2 && f, TOffset offset, TValue value)
            : TransferState<TOffset, TValue>(offset, value)
            , transform(static_cast<F2&&>(f))
            {
                
            }

            F transform;
        };

    public:
        I2cMemory(I2cDevice & i2cDevice, std::uint8_t slaveAddress)
        : i2cDevice_(i2cDevice), slaveAddress_(slaveAddress)
        {

        }

        template<class T, class TOffset, TOffset addr>
        async::Future<T, I2cError> auto read(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>)
        {
            return async::useState(
                TransferState<TOffset, T>{addr},
                [this](auto & state) {
                    return 
                        i2cDevice_.writeAndRead(slaveAddress_, 
                            state.addressBuffer(), state.addressSize(), 
                            state.valueBuffer(), state.valueSize())
                        | async::map([&]() -> T { 
                            return state.getValue(); 
                        });
                });
        }

        template<class T, class TOffset, TOffset addr, class F>
        async::Future<void, I2cError> auto readAndTransform(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, F && f)
        {
            return async::useState(
                ReadModifyWriteState<TOffset, T, std::remove_cvref_t<F>>{static_cast<F&&>(f), addr}, 
                [this](auto & state) {
                    return 
                        i2cDevice_.writeAndRead(
                            slaveAddress_, 
                            state.addressBuffer(), 
                            state.addressSize(), 
                            state.valueBuffer(), 
                            state.valueSize())
                        | async::map([&]() {
                            return std::move(state.transform)(*(state.getValuePtr()));
                        });
                });
        }

        template<class T, class TOffset, TOffset addr, class T2>
        async::Future<void, I2cError> auto write(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, T2 value)
        {
            return async::useState(
                TransferState<TOffset, T>{addr, static_cast<T>(value)},
                [this](auto & state) {
                    return i2cDevice_.write(slaveAddress_, state.buffer, state.bufferSize());
                });
        }

        template<class T, class TOffset, TOffset addr, class F>
        async::Future<void, I2cError> auto readModifyWrite(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, F && f)
        {
            return 
                async::useState(
                    ReadModifyWriteState<TOffset, T, std::remove_cvref_t<F>>{
                        static_cast<F&&>(f), 
                        addr}, 
                    [this](auto & state) {
                        return async::sequence(
                            i2cDevice_.writeAndRead(slaveAddress_, 
                                state.addressBuffer(), state.addressSize(), 
                                state.valueBuffer(), state.valueSize())
                            // Overwrite the value part of the buffer with the result of `f`
                            | async::map([&]() {
                                state.transform([&](T result) {
                                    *(state.getValuePtr()) = result;
                                }, state.getValue());
                            }),
                            i2cDevice_.write(slaveAddress_, state.buffer, state.bufferSize()));
                    });       
        }

        /*template<type addr, class F>
        async::Sender auto readModifyWriteTransform(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, F && f)
        {

        }*/

    private:
        I2cDevice & i2cDevice_;
        std::uint8_t slaveAddress_;
    };
}