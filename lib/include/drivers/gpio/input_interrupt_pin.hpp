#pragma once
#include "types.hpp"
#include "board/regmap/exti.hpp"
#include "board/regmap/gpio.hpp"
#include "async/event.hpp"
#include "async/make_stream.hpp"
#include "async/receiver.hpp"
#include <type_traits>

#include "reg/read.hpp"

namespace drivers::gpio
{
    enum class GpioError
    {
        BUSY
    };

    namespace detail
    {
        template<class R, class GpioX, class Exti, std::uint8_t pinNo>
        class WhenChangedOperation : public async::EventHandlerImpl<WhenChangedOperation<R, GpioX, Exti, pinNo>>
        {
            enum State
            {
                WAITING_FOR_VALUE,
                RECEIVED_TRUE,
                RECEIVED_FALSE
            };
            
        public:
            template<class R2>
            WhenChangedOperation(R2 && receiver, const async::EventEmitter & interruptEvent) 
            : receiver_(static_cast<R2&&>(receiver))
            , interruptEvent_(interruptEvent) 
            { 

            }

            void start()
            {
                if (!interruptEvent_.subscribe(this))
                {
                    async::setError(std::move(receiver_), GpioError::BUSY);
                    return;
                }

                reg::set(Exti{}, board::exti::IMR::MR[uint8_c<pinNo>]);
            }

            void next()
            {
                if (state_ == State::RECEIVED_TRUE)
                {
                    state_ = State::WAITING_FOR_VALUE;
                    async::setNext(receiver_, true);
                }
                else if (state_ == State::RECEIVED_FALSE)
                {
                    state_ = State::WAITING_FOR_VALUE;
                    async::setNext(receiver_, false);
                }
                else
                {
                    state_ = State::WAITING_FOR_VALUE;
                }
            }

            void handleEvent()
            {
                bool value = reg::read(GpioX{}, board::gpio::IDR::IDR[uint8_c<pinNo>]);
                reg::set(Exti{}, board::exti::PR::PR[uint8_c<pinNo>]);
                
                /*if (state_ == State::WAITING_FOR_VALUE)
                {
                    auto & s = async::getScheduler(receiver_);
                    if (value)
                        s.postFromISR({memFn<&WhenChangedOperation::emitHigh>, *this});
                    else
                        s.postFromISR({memFn<&WhenChangedOperation::emitLow>, *this});
                }*/
            }

            void stop()
            {
                reg::clear(Exti{}, board::exti::IMR::MR[uint8_c<pinNo>]);
                interruptEvent_.unsubscribe();
                async::setDone(std::move(receiver_));
            }

            void emitHigh() { async::setNext(receiver_, true); }
            void emitLow() { async::setNext(receiver_, false); }

        private:
            [[no_unique_address]] R receiver_;
            async::EventEmitter interruptEvent_;
            volatile State state_;
        };
    }

    template<class GpioX, class Exti, std::uint8_t portNo, std::uint8_t pinNo>
    class InputInterruptPin
    {
        async::EventEmitter interruptEvent_;

    public:
        InputInterruptPin(const async::EventEmitter & interruptSource) : interruptEvent_(interruptSource) { }

        bool read()
		{
			return reg::read(GpioX{}, board::gpio::IDR::IDR[uint8_c<pinNo>]);
		}

        async::Stream<bool, GpioError> auto whenChanged()
        {
            return async::makeStream<bool, GpioError>(
                [this]<typename R>(R && receiver) 
                    -> detail::WhenChangedOperation<std::remove_cvref_t<R>, GpioX, Exti, pinNo> 
                {
                    return {static_cast<R&&>(receiver), interruptEvent_};
                });
        }

        /*constexpr auto whenChanged()
        {
            return async::transform(
                async::withEffect(
                    interruptSenderFactory_.getManySender(),
                    []() {  }
                ),
                [this]() {
                    bool value = read();
                    reg::set(Exti{}, board::exti::PR::PR[uint8_c<pinNo>]);
                    return value;
                });
        }

        constexpr auto awaitChange()
        {
            return async::transform(
                async::withEffect(
                    interruptSenderFactory_.getSender(),
                    []() { reg::set(Exti{}, board::exti::IMR::MR[uint8_c<pinNo>]); }
                ),
                [this]() {
                    bool value = read();
                    reg::clear(Exti{}, board::exti::IMR::MR[uint8_c<pinNo>]);
                    reg::set(Exti{}, board::exti::PR::PR[uint8_c<pinNo>]);
                    return value;
                });
        }*/
    };  
}