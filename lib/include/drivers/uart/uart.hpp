#pragma once
#include "board/regmap/uart.hpp"
#include "async/event.hpp"
#include "uart_error.hpp"
#include "async/make_future.hpp"
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
        async::Future<void, UartError> auto write(const std::uint8_t * data, std::uint32_t size)
        {
            return async::makeFuture<void, UartError>(
                [this, data, size]<typename R>(R && receiver) mutable
                    -> detail::WriteOperation<UartX, std::remove_cvref_t<R>>
                {
                    return { static_cast<R&&>(receiver), interruptSource_, data, size };
                });
        }
    private:
        async::EventEmitter interruptSource_;
    };
}