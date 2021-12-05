#pragma once
#include "async/receiver.hpp"
#include "async/sender.hpp"

namespace drivers::spi::detail
{
    template<class DataType, class Receiver>
    class WriteDmaOperator
    {
    public:
        template<template<typename...> class Variant, template<typename...> class Tuple>
        using next_types = Variant<Tuple<DataType *>>;

        void start()
        {
            async::start(dmaOperation_);
            reg::set(spiX, board::spi::CR2::TXDMAEN);
        }

        void setNext()
        {
            async::setNext()
        }

        template<class E>
        void setError(E && e) &&
        {

        }

    private:
        void stop()
        {
            reg::clear(spiX, board::spi::CR2::TXDMAEN);
        }

        DataType * data_[2];
        Receiver receiver_;
    };
}