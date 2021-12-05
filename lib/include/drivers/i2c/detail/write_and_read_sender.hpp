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
    public:
        template<template<typename ...> class Variant, template<typename ...> class Tuple>
        using value_types = Variant<Tuple<>>;
        template<template<typename ...> class Variant>
        using error_types = Variant<I2cError>;

        template<class R>
        auto connect(R && receiver) &&
            -> TransactionOperation<I2cX, WriteAndReadEventHandler<I2cX>, ErrorEventHandler<I2cX>, std::remove_cvref_t<R>>
        {
            return {
                static_cast<R&&>(receiver), 
                eventInterrupt_, 
                errorInterrupt_, 
                slaveAddress_, 
                WriteAndReadEventHandler<I2cX>{writeData_, writeSize_, readBuffer_, readSize_},
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