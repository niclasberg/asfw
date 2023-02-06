#pragma once
#include "async/event.hpp"
#include "async/receiver.hpp"
#include "../i2c_error.hpp"
#include "reg/peripheral_operations.hpp"
#include "board/regmap/i2c.hpp"
#include "async/scheduler.hpp"

#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/apply.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::i2c::detail
{
    template <class I2cX, class _EventHandler, class _ErrorHandler, class R>
    class TransactionOperation
    {
        //static_assert(async::hasScheduler<R>, "A scheduler must be bound to the receiver");

        struct EventInterruptHandler : async::EventHandlerImpl<EventInterruptHandler>
        {
            EventInterruptHandler(_EventHandler eventHandler, std::uint8_t slaveAddress, TransactionOperation & parent)
            : eventHandler_(eventHandler), slaveAddress_(slaveAddress), parent_(parent)
            {

            }

            void handleEvent() volatile
            {
                eventHandler_(parent_, slaveAddress_);
            }

            _EventHandler eventHandler_;
            const std::uint8_t slaveAddress_;
            TransactionOperation & parent_;
        };

        struct ErrorInterruptHandler : async::EventHandlerImpl<ErrorInterruptHandler>
        {
            ErrorInterruptHandler(_ErrorHandler errorHandler, TransactionOperation & parent)
            : errorHandler_(errorHandler), parent_(parent)
            {
                
            }

            void handleEvent() volatile
            {
                errorHandler_(parent_);
            }

            _ErrorHandler errorHandler_;
            TransactionOperation & parent_;
        };

    public:
        template<class R2>
        TransactionOperation(
            R2 && receiver, 
            const async::EventEmitter & eventInterrupt,
            const async::EventEmitter & errorInterrupt,
            std::uint8_t slaveAddress,
            _EventHandler eventHandler,
            _ErrorHandler errorHandler)
        : receiver_(static_cast<R2&&>(receiver))
        , eventInterrupt_(eventInterrupt)
        , errorInterrupt_(errorInterrupt)
        , eventInterruptHandler_{eventHandler, slaveAddress, *this}
        , errorInterruptHandler_{errorHandler, *this}
        {

        }

        void start()
        {
            if (!eventInterrupt_.subscribe(&eventInterruptHandler_))
            {
                async::setError(std::move(receiver_), I2cError::BUSY);
                return;
            }

            if (!errorInterrupt_.subscribe(&errorInterruptHandler_))
            {
                eventInterrupt_.unsubscribe();
                async::setError(std::move(receiver_), I2cError::BUSY);
                return;
            }

            startImpl();
        }

        void stop()
        {
            reg::apply(I2cX{},
                reg::clear(board::i2c::CR2::ITBUFEN),
                reg::clear(board::i2c::CR2::ITEVTEN),
                reg::clear(board::i2c::CR2::ITERREN));
            
            eventInterrupt_.unsubscribe();
            errorInterrupt_.unsubscribe();
        }

        void finishTransactionWithValue()
        {
            stop();
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&TransactionOperation::setValue>, *this});
        }

        void finishTransactionWithError(I2cError error)
        {
            stop();
            error_ = error;
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&TransactionOperation::setError>, *this});
        }

    private:
        void startImpl()
        {
            if (reg::bitIsSet(I2cX{}, board::i2c::SR2::BUSY))
            {
                // Retry 
                auto & s = async::getScheduler(receiver_);
                s.postFromISR({memFn<&TransactionOperation::startImpl>, *this});
                return;
            }

            // Generate start condition (the rest is handled in the IRQ handler)
            reg::set(I2cX{}, board::i2c::CR1::START);

            // Interrupt on SB, ADDR, ADD10, STOPF, BTF and Error 
            reg::apply(I2cX{},
                reg::set(board::i2c::CR2::ITEVTEN),
                reg::set(board::i2c::CR2::ITERREN)); 
        }

        void setValue()
        {
            async::setValue(std::move(receiver_), tmp::Void{});
        }

        void setError()
        {
            async::setError(std::move(receiver_), error_);
        }

        R receiver_;
        async::EventEmitter eventInterrupt_;
        async::EventEmitter errorInterrupt_;
        EventInterruptHandler eventInterruptHandler_;
        ErrorInterruptHandler errorInterruptHandler_;
        volatile I2cError error_;
    };
}