#pragma once
#include "board/regmap/uart.hpp"
#include "async/event.hpp"
#include "uart_error.hpp"
#include "async/make_source_sender.hpp"
#include "detail/write.hpp"

namespace drivers::uart
{   
    template<class UartX>
    class Uart
    {
    public:
        explicit Uart(const async::EventEmitter & interruptSource) 
        : interruptSource_(interruptSource) 
        { 

        }

        Uart(const Uart &) = delete;
        Uart(Uart &&) = delete;
        Uart & operator=(const Uart &) = delete;
        Uart & operator=(Uart &&) = delete;

        /**
         * Write data to the peripheral in non-blocking (interrupt) mode.
         * 
         * @param data Array of data to write
         * @param size Array size
         * @return void sender
         */
        auto write(const std::uint8_t * data, std::uint32_t size)
        {
            return async::makeSourceSender<UartError>(
                [this, data, size]<typename R>(R && receiver) -> detail::WriteOperation<UartX, std::remove_cvref_t<R>>
                {
                    return {static_cast<R&&>(receiver), interruptSource_, data, size};
                });
        }

        /*bool write(const std::uint8_t * data, std::uint32_t size)
        {
            while(size > 0)
            {
                while(!reg::bitIsSet(uartX, board::uart::SR::TXE)) { }
                uartX.write(board::uart::DR::_Offset{}, (*data++) & 0xFF);
                --size;
            }

            while(!reg::bitIsSet(uartX, board::uart::SR::TC)) { }

            return true;
        }*/
    private:
        async::EventEmitter interruptSource_;
    };
}