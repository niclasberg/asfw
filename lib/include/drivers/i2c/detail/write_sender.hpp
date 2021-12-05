#pragma once
#include "transaction_operation.hpp"
#include "write_event_handler.hpp"
#include "error_event_handler.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class WriteSender
    {
    public:
        template<template<typename ...> class Tuple, template<typename ...> class Variant>
        using value_types = Tuple<Variant<>>;
        template<template<typename ...> class Variant>
        using error_types = Variant<I2cError>;

        template<class R>
        auto connect(R && receiver) &&
            -> TransactionOperation<
                I2cX, 
                WriteEventHandler<I2cX, false>, 
                ErrorEventHandler<I2cX>,
                std::remove_cvref_t<R>>
        {
            return {
                static_cast<R&&>(receiver), 
                eventInterrupt_, 
                errorInterrupt_, 
                slaveAddress_, 
                WriteEventHandler<I2cX, false>{data_, size_},
                ErrorEventHandler<I2cX>{}};
        }

        std::uint8_t slaveAddress_;
        async::EventEmitter eventInterrupt_;
        async::EventEmitter errorInterrupt_;
        const std::uint8_t * data_;
        const std::uint16_t size_;
    };
}