#pragma once
#include "peripheral_types.hpp"
#include "board/interrupts.hpp"
#include "spi.hpp"
#include "detail/interrupt.hpp"

#include "types.hpp"
#include "detail/bus_config.hpp"
#include "board/regmap/spi.hpp"
#include "reg/peripheral_operations.hpp"
#include "reg/apply.hpp"
#include "reg/write.hpp"

namespace drivers::spi
{
    enum class DeviceMode
    {
        MASTER,
        SLAVE
    };

    enum class BitOrder
    {
        LSB_FIRST,
        MSB_FIRST
    };

    using DataFrameFormat = board::spi::CR1::DffVal;

    // Bus config
    using BusConfig = detail::BusConfig;

    // Clock polarity
    using ClockPolarity = board::spi::CR1::CpolVal;

    // Clock phase
    using ClockPhase = board::spi::CR1::CphaVal;

    // The factor that the peripheral clock should be divided by to obtain the SPI baud rate
    using BaudRateDivider = board::spi::CR1::BrVal;

    struct SpiConfig
    {
        std::uint8_t deviceId;
        /* Data pin: output for master, input for slave*/
        Pin mosiPin;
        /* Data pin: input for master, output for slave */
        Pin misoPin;
        DeviceMode deviceMode = DeviceMode::MASTER;
        BusConfig busConfig = BusConfig::FULL_DUPLEX;
        BitOrder bitOrder = BitOrder::LSB_FIRST;
        DataFrameFormat dataFrameFormat = DataFrameFormat::BYTE;
        BaudRateDivider baudRateDivider = BaudRateDivider::PCKL_DIV2;
        ClockPhase clockPhase = ClockPhase::LOW;
        ClockPolarity clockPolarity = ClockPolarity::LOW;
    };

    template<SpiConfig config>
    struct makeSpi_t final
    {
        template<class Board>
        constexpr auto operator()(Board boardDescriptor) const
        {
            using DataType = std::conditional_t<
                config.dataFrameFormat == DataFrameFormat::BYTE,
                std::uint8_t,
                std::uint16_t>;

            constexpr auto spiX = boardDescriptor
                .getPeripheral(PeripheralTypes::SPI<config.deviceId>); 

            spiX.enable();

            reg::apply(spiX,
                reg::write(board::spi::CR1::BR, constant_c<config.baudRateDivider>),
                reg::write(board::spi::CR1::CPOL, constant_c<config.clockPolarity>),
                reg::write(board::spi::CR1::CPHA, constant_c<config.clockPhase>),
                reg::write(board::spi::CR1::DFF, constant_c<config.dataFrameFormat>),
                reg::write(board::spi::CR1::LSBFIRST, bool_c<config.bitOrder == BitOrder::LSB_FIRST>),
                reg::write(board::spi::CR1::MSTR, bool_c<config.deviceMode == DeviceMode::MASTER>));

            // Figure out NSS pin handling and put here
            if constexpr (config.busConfig == BusConfig::FULL_DUPLEX)
            {
                
            }

            reg::write(spiX, board::spi::CR1::SPE, true_c);

            auto interrupt = detail::getInterrupt<config.deviceId>();
            boardDescriptor.enableIRQ(interrupt);

            return Spi<decltype(spiX), DataType, config.busConfig>{
                boardDescriptor.getInterruptEvent(interrupt) };
        }
    };

    template<SpiConfig config>
    inline constexpr makeSpi_t<config> makeSpi {};
}