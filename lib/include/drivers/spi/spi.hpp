#pragma once
#include "async/event.hpp"
#include "async/make_future.hpp"
#include "spi_error.hpp"
#include "detail/bus_config.hpp"
#include "detail/read.hpp"
#include "detail/write.hpp"

namespace drivers::spi
{
    namespace detail
    {
        template<detail::BusConfig busConfig>
        constexpr detail::WriteOperationType getWriteOpType()
        {
            if constexpr (busConfig == detail::BusConfig::FullDuplex)
                return detail::WriteOperationType::FULL_DUPLEX_SPI;
            else if constexpr (busConfig == detail::BusConfig::HalfDuplex)
                return detail::WriteOperationType::HALF_DUPLEX_SPI;
            else if constexpr (busConfig == detail::BusConfig::TxOnly)
                return detail::WriteOperationType::TX_ONLY_SPI;
        }
    }

    template<class SpiX, class DataType, detail::BusConfig busConfig>
    class Spi
    {
    public:
        explicit Spi(const async::EventEmitter & interruptSource) : interruptSource_(interruptSource) { }

        Spi(const Spi &) = delete;
        Spi(Spi &&) = delete;
        Spi & operator=(const Spi &) = delete;
        Spi & operator=(Spi &&) = delete;

        /**
         * Write data to the peripheral in non-blocking (interrupt) mode.
         * 
         * @param data Array of data to write
         * @param size Array size
         * @return void sender
         */
        async::Future<void, SpiError> auto write(const DataType * data, std::uint32_t size)
        {
            return async::makeFuture<void, SpiError>(
                [this, data, size]<typename R>(R && receiver) 
                    -> detail::WriteOperation<SpiX, DataType, detail::WriteOperationType::FULL_DUPLEX_SPI, std::remove_cvref_t<R>>
                {
                    return {static_cast<R&&>(receiver), interruptSource_, data, size};
                });
        }

        /**
         * Read data from the peripheral in non-blocking (interrupt) mode.
         * 
         * @param data Array to store the read data into
         * @param size Array size
         * @return SenderOf<DataType *, std::uint32_t>
         */
        async::Future<void, SpiError> auto read(DataType * data, std::uint32_t size)
        {
            return async::makeFuture<void, SpiError>(
                [=]<typename R>(R && receiver) 
                    -> detail::ReadOperation<SpiX, DataType, std::remove_cvref_t<R>>
                {
                    return {static_cast<R&&>(receiver), interruptSource_, data, size};
                });
        }

    private:
        async::EventEmitter interruptSource_;
    };
}