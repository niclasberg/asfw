#pragma once
#include "transaction_operation.hpp"
#include "write_event_handler.hpp"
#include "error_event_handler.hpp"
#include "async/future.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class WriteSender
    {
        using Self = WriteSender<I2cX>;
    public:
        using value_type = void;
        using error_type = I2cError;

        template<class R>
        friend auto tag_invoke(async::connect_t, const Self & self, R && receiver)
            -> TransactionOperation<
                I2cX, 
                WriteEventHandler<I2cX, false>, 
                ErrorEventHandler<I2cX>,
                std::remove_cvref_t<R>>
        {
            return {
                static_cast<R&&>(receiver), 
                self.eventInterrupt_, 
                self.errorInterrupt_, 
                self.slaveAddress_, 
                WriteEventHandler<I2cX, false>{self.data_, self.size_},
                ErrorEventHandler<I2cX>{}};
        }

        std::uint8_t slaveAddress_;
        async::EventEmitter eventInterrupt_;
        async::EventEmitter errorInterrupt_;
        const std::uint8_t * data_;
        const std::uint16_t size_;
    };
}