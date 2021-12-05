#pragma once
#include "i2c_like.hpp"
#include "reg/field.hpp"
#include <array>
#include "async/receiver.hpp"
#include "async/sender.hpp"
#include "async/use_state.hpp"
#include "async/then.hpp"
#include "async/transform.hpp"
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

        template<class R, class TValue, class F>
        struct ReadTransformReceiver
        {
            void setValue() &&
            {
                f_(
                    [this](auto, auto... values) { async::setValue(static_cast<R&&>(innerReceiver_), values...); },
                    valueRef_);
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(innerReceiver_), static_cast<E&&>(e));
            }

            void setDone() && 
            {
                async::setDone(std::move(innerReceiver_));
            }

            template<class Cpo, class Self, class ... Args>
                requires (std::same_as<std::decay_t<Self>, ReadTransformReceiver>)
            friend auto tag_invoke(Cpo cpo, Self && self, Args &&... args) 
                -> tmp::CallableResultType<Cpo, decltype(std::declval<Self&&>().innerReceiver_), decltype(args)&&...>
            {
                return cpo(static_cast<Self&&>(self).innerReceiver_, static_cast<Args&&>(args)...);
            }

            R innerReceiver_;
            TValue & valueRef_;
            F f_;
        };

        /*template<class TValue, class F>
        struct ReadValueTransformOperator
        {
            template<
                class SenderType,
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using value_types = Variant<typename F::template ValueTypes<Tuple>>;
            
            template<class SenderType,template<typename...> typename Variant> 
            using error_types = typename ParentSender::template error_types<Variant>;

            void operator()(setValue_t)
            {
                f_(
                    [this](auto, auto... values) { async::setValue(static_cast<R&&>(receiver), values...); },
                    valueRef_);
            }

            template<class E>
            void operator()(setError_t, E && e)
            {
                async::setError(static_cast<R&&>(receiver), static_cast<E&&>(e));
            }

            void operator()(setDone_t)
            {
                async::setDone(static_cast<R&&>(receiver));
            }

            F f_;
            TValue & valueRef_;
        };*/

        template<class ParentSender, class TValue, class F>
        struct ReadTransformSender
        {
            template<class R>
            using ReceiverType = ReadTransformReceiver<std::remove_cvref_t<R>, TValue, F>;

            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using value_types = Variant<typename F::template ValueTypes<Tuple>>;
            
            template<template<typename...> typename Variant> 
            using error_types = typename ParentSender::template error_types<Variant>;

            template<class R>
            auto connect(R && receiver) && -> async::ConnectResultType<ParentSender, ReceiverType<R>>
            {
                return async::connect(
                    std::move(parentSender_),
                    ReceiverType<R>{static_cast<R&&>(receiver), valueRef_, std::move(f_)});
            }

            ParentSender parentSender_;
            TValue & valueRef_;
            F f_;
        };

        template<class S, class T, class F>
        auto transformReadValue(S && sender, T & valueRef, F && action)
            -> ReadTransformSender<std::remove_cvref_t<S>, T, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(sender), valueRef, static_cast<F&&>(action)};
        }
    }

    template<
        I2cLike I2cDevice,
        class RegTag, 
        class Serializer = detail::DefaultAddressSerializer>
    class I2cMemory
    {
    public:
        I2cMemory(I2cDevice & i2cDevice, std::uint8_t slaveAddress)
        : i2cDevice_(i2cDevice), slaveAddress_(slaveAddress)
        {

        }

        template<class T, class TOffset, TOffset addr>
        async::Sender auto read(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>)
        {
            // Use the first part of the internal buffer for 
            // the address, and the second as an input buffer for reading
            Serializer::serializeAddress(addr, transmitBuffer_);

            return i2cDevice_.writeAndRead(slaveAddress_, 
                        transmitBuffer_, sizeof(TOffset), 
                        transmitBuffer_+sizeof(TOffset), sizeof(T))
                    | async::transform([this]() -> T { 
                        return *std::launder(reinterpret_cast<T *>(transmitBuffer_+sizeof(TOffset))); 
                    });
        }

        template<class T, class TOffset, TOffset addr, class F>
        async::Sender auto readAndTransform(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, F && f)
        {
            return 
                async::useState(
                    detail::ReadRequest<TOffset, T>{addr, T{}}, 
                    [this, f = static_cast<F&&>(f)](auto & readRequest) {
                        return 
                            detail::transformReadValue(
                                i2cDevice_.writeAndRead(
                                    slaveAddress_, 
                                    reinterpret_cast<std::uint8_t *>(&readRequest.address), sizeof(TOffset), 
                                    reinterpret_cast<std::uint8_t *>(&readRequest.value), sizeof(T)),
                                readRequest.value,
                                std::move(f));
                    });
        }

        template<class T, class TOffset, TOffset addr, class T2>
        async::Sender auto write(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, T2 value)
        {
            setBufferForWriting<TOffset, T>(addr, value);
            return i2cDevice_.write(slaveAddress_, transmitBuffer_, sizeof(T)+sizeof(TOffset));
        }

        template<class T, class TOffset, TOffset addr, class F>
        async::Sender auto readModifyWrite(reg::FieldLocation<T, RegTag, reg::FieldOffset<TOffset, addr>>, F && f)
        {
            Serializer::serializeAddress(addr, transmitBuffer_);

            return 
                async::sequence(
                    i2cDevice_.writeAndRead(slaveAddress_, 
                        transmitBuffer_, sizeof(TOffset), 
                        transmitBuffer_+sizeof(TOffset), sizeof(T))
                    // Overwrite the value part of the buffer with the result of `f`
                    | async::transform([this, ff=std::move(f)]() mutable {
                        T * valuePtr = std::launder(reinterpret_cast<T *>(transmitBuffer_+sizeof(TOffset)));
                        ff([valuePtr](T result) {
                            *valuePtr = result;
                        }, *valuePtr);
                    }),
                    i2cDevice_.write(slaveAddress_, transmitBuffer_, sizeof(T)+sizeof(TOffset)));
        }

        /*template<type addr, class F>
        async::Sender auto readModifyWriteTransform(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, F && f)
        {

        }*/

    private:
        template<class TOffset, class T>
        void setBufferForWriting(TOffset offset, T val)
        {
            static_assert((sizeof(TOffset) + sizeof(T)) < sizeof(transmitBuffer_),
                "Register address and value does not fit into the internal buffer");
            
            // Message: address0 | ... | addressN | data0 | ... | dataM
            Serializer::serializeAddress(offset, transmitBuffer_);
            *std::launder(reinterpret_cast<T *>(transmitBuffer_ + sizeof(TOffset))) = val;
        }

        I2cDevice & i2cDevice_;
        std::uint8_t slaveAddress_;
        std::uint8_t transmitBuffer_[16];
    };
}