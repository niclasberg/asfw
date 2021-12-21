#pragma once
#include "async/event.hpp"
#include "../uart_error.hpp"
#include "async/receiver.hpp"
#include "reg/peripheral_operations.hpp"
#include "board/regmap/uart.hpp"
#include <cstdint>

#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/apply.hpp"
#include "reg/unchecked_write.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::uart
{
    namespace detail
    {
        template<class UartX, class R>
        class WriteOperation : public async::EventHandlerImpl<WriteOperation<UartX, R>>
        {
        public:
            template<class R2>
            WriteOperation(
                R2 && receiver, 
                const async::EventEmitter & interruptEvent,
                const std::uint8_t * data,
                std::uint32_t size)
            : receiver_(static_cast<R2&&>(receiver))
            , interruptEvent_(interruptEvent)
            , current_(data)
            , end_(data + size)
            {

            }

            void start()
            {
                if(current_ == end_)
                {
                    async::setValue(std::move(receiver_));
                    return;
                }

                if (!interruptEvent_.subscribe(this))
                {
                    async::setError(std::move(receiver_), UartError::BUSY);
                    return;
                }

                reg::apply(UartX{}, 
                    reg::set(board::uart::CR1::TXEIE),
                    reg::set(board::uart::CR1::TCIE));
            }

            void handleEvent()
            {
                if (current_ < end_)
                {
                    if (reg::bitIsSet(UartX{}, board::uart::SR::TXE))
                    {
                        reg::uncheckedWrite(UartX{}, board::uart::DR::_Offset{}, *(current_++));
                        return;
                    }
                }
                else
                {
                    // Wait for transfer complete
                    if (reg::bitIsSet(UartX{}, board::uart::SR::TC))
                    {
                        stop();
                        async::setValue(std::move(receiver_));
                        return;
                    }
                }

                // Handle error interrupt
            }

            void stop()
            {
                reg::apply(UartX{}, 
                    reg::clear(board::uart::CR1::TXEIE),
                    reg::clear(board::uart::CR1::TCIE));
                interruptEvent_.unsubscribe();
            }

        private:
            R receiver_;
            async::EventEmitter interruptEvent_;
            const std::uint8_t * current_;
            const std::uint8_t * end_;
        };
    }
}