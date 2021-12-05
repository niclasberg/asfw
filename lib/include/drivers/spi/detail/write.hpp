#pragma once
#include "async/receiver.hpp"
#include "async/stop_token.hpp"
#include "async/event.hpp"
#include "async/receiver.hpp"
#include "async/scheduler.hpp"
#include "../spi_error.hpp"
#include "board/regmap/spi.hpp"
#include "traits_helper.hpp"
#include "reg/peripheral_operations.hpp"
#include "reg/unchecked_read.hpp"
#include "reg/unchecked_write.hpp"

#include <type_traits>
#include <cstdint>

namespace drivers::spi::detail
{
    /**
     * Base write operation. The Derived class can implement the following static functions:
     * - void onStart(): called before interrupts are enabled and transmission is started
     * - void onStop(): called once the transmission is finished, and after interrupts are disabled
     * - void onEvent(): called in the interrupt handler, after a value has been transmitted
     */
    template<class SpiX, class DataType, async::VoidReceiver R, class Hooks>
    class WriteOperation : async::EventHandlerImpl<WriteOperation<SpiX, DataType, R, Hooks>>
    {
    public:
        template<class R2>
        WriteOperation(
            R2 && receiver, 
            const async::EventEmitter & interruptEvent,
            const DataType * data,
            std::uint32_t size)
        : receiver_(static_cast<R2&&>(receiver))
        , interruptEvent_(interruptEvent)
        , data_(data)
        , currentIndex_(0U)
        , size_(size)
        {

        }

        WriteOperation(const WriteOperation &) = delete;
        WriteOperation& operator=(const WriteOperation &) = delete;

        void start()
        {
            if(size_ == 0)
            {
                async::setValue(std::move(receiver_));
                return;
            }

            if constexpr (hasOnStart<Hooks>) 
            {
                Hooks::onStart();
            }

            if (!interruptEvent_.subscribe(this))
            {
                async::setError(std::move(receiver_), SpiError::BUSY);
                return;
            }

            reg::apply(SpiX{},
                reg::set(board::spi::CR2::TXEIE),
                reg::set(board::spi::CR2::ERRIE));
        }

        void handleEvent()
        {
            if (reg::bitIsSet(SpiX{}, board::spi::SR::TXE))
            {
                if (currentIndex_ < size_)
                {
                    reg::uncheckedWrite(SpiX{}, board::spi::DR::_Offset{}, data_[currentIndex_]);
                    currentIndex_ = currentIndex_ + 1;
                }
                else
                {
                    stop();
                    setValueFromISR();
                }
                return;
            }

            if constexpr (hasOnEvent<Hooks>) 
            {
                Hooks::onEvent();
            }

            if (reg::bitIsSet(SpiX{}, board::spi::SR::MODF))
            {
                stop();
                setErrorFromISR(SpiError::MODE_FAULT);
            }
        }

        void stop()
        {
            reg::apply(SpiX{}, 
                reg::clear(board::spi::CR2::TXEIE),
                reg::clear(board::spi::CR2::ERRIE));
            interruptEvent_.unsubscribe();

            if constexpr (hasOnStop<Hooks>) 
            {
                Hooks::onStop();
            }
        }

    private:
        void setValueFromISR()
        {
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&WriteOperation::setValueImpl>, *this});
        }

        void setValueImpl()
        {
            async::setValue(std::move(receiver_));
        }

        void setErrorFromISR(SpiError error)
        {
            error_ = error;
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&WriteOperation::setErrorImpl>, *this});
        }

        void setErrorImpl()
        {
            async::setError(std::move(receiver_), error_);
        }

        R receiver_;
        async::EventEmitter interruptEvent_;
        const DataType * data_;
        volatile std::uint32_t currentIndex_;
        const std::uint32_t size_;
        volatile SpiError error_;
    };

    template<class SpiX>
    struct FullDuplexWriteHooks
    {
        static void onStop()
        {
            // Clear the overrun flag by reading the dataregister, followed by the status register
            reg::uncheckedRead(SpiX{}, board::spi::DR::_Offset{});
            reg::uncheckedRead(SpiX{}, board::spi::SR::_Offset{});
        }
    };

    template<class SpiX, class DataType, async::VoidReceiver R>
    using FullDuplexWriteOperation = WriteOperation<SpiX, DataType, R, FullDuplexWriteHooks<SpiX>>;

    template<class SpiX>
    struct HalfDuplexWriteHooks
    {
        static void onStart()
        {
            reg::set(SpiX{}, board::spi::CR1::BIDIOE);
        }
    };

    template<class SpiX, class DataType, async::VoidReceiver R>
    using HalfDuplexWriteOperation = WriteOperation<SpiX, DataType, R, HalfDuplexWriteHooks<SpiX>>;

    struct TxOnlyWriteHooks
    {
        
    };

    template<class SpiX, class DataType, async::VoidReceiver R>
    using TxOnlyWriteOperation = WriteOperation<SpiX, DataType, R, TxOnlyWriteHooks>;
}