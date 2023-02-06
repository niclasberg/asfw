#pragma once
#include "read_event_handler.hpp"
#include "transaction_operation.hpp"
#include "error_event_handler.hpp"
#include "async/future.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class ReadSender
    {
        using Self = ReadSender<I2cX>;
    public:
        using value_type = void;
        using error_type = I2cError;

        template<class R>
        friend auto tag_invoke(async::connect_t, const Self & sender, R && receiver) 
            -> TransactionOperation<I2cX, ReadEventHandler<I2cX, false>, ErrorEventHandler<I2cX>, std::remove_cvref_t<R>>
        {
            return {
                static_cast<R&&>(receiver), 
                sender.eventInterrupt_, 
                sender.errorInterrupt_, 
                sender.slaveAddress_, 
                ReadEventHandler<I2cX, false>{sender.data_, sender.size_},
                ErrorEventHandler<I2cX>{}};
        }

        std::uint8_t slaveAddress_;
        async::EventEmitter eventInterrupt_;
        async::EventEmitter errorInterrupt_;
        std::uint8_t * data_;
        const std::uint16_t size_;
    };
}