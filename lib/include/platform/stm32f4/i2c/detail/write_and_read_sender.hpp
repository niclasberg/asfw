#pragma once
#include "../i2c_error.hpp"
#include "transaction_operation.hpp"
#include "write_and_read_event_handler.hpp"
#include "error_event_handler.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class WriteAndReadSender
    {
        using Self = WriteAndReadSender<I2cX>;
    public:
        using value_type = void;
        using error_type = I2cError;

        template<class R>
        friend auto tag_invoke(async::connect_t, const Self & self, R && receiver)
            -> TransactionOperation<I2cX, WriteAndReadEventHandler<I2cX>, ErrorEventHandler<I2cX>, std::remove_cvref_t<R>>
        {
            return {
                static_cast<R&&>(receiver), 
                self.eventInterrupt_, 
                self.errorInterrupt_, 
                self.slaveAddress_, 
                WriteAndReadEventHandler<I2cX>{self.writeData_, self.writeSize_, self.readBuffer_, self.readSize_},
                ErrorEventHandler<I2cX>{}};
        }

        const std::uint8_t slaveAddress_;
        async::EventEmitter eventInterrupt_;
        async::EventEmitter errorInterrupt_;
        const std::uint8_t * writeData_;
        const std::uint16_t writeSize_;
        std::uint8_t * readBuffer_;
        const std::uint16_t readSize_;
    };
}