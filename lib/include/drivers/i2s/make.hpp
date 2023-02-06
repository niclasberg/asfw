#pragma once
#include "detail/init.hpp"
#include "i2s.hpp"
#include <utility>
#include "board/clock/config.hpp"
#include "drivers/spi/detail/interrupt.hpp"
#include "drivers/gpio/make.hpp"
#include "peripheral_types.hpp"

namespace drivers::i2s
{
    // I2S Standard
    using Standard = board::spi::I2SCFGR::I2sStdVal;

    using BitDepth = board::spi::I2SCFGR::ChLenVal;
    using DataFrameFormat = board::spi::I2SCFGR::DataLenVal;
    using ClockPolarity = board::spi::I2SCFGR::CkPolVal;

    enum class TransferMode
    {
        TX_ONLY,
        RX_ONLY
    };

    struct MasterConfig
    {
        std::uint8_t id;
        Pin serialDataPin;
        Pin serialClockPin;
        Pin wordSelectPin;
        Pin masterClockPin;
        std::uint32_t sampleRate;
        TransferMode transferMode = TransferMode::TX_ONLY;
        Standard standard = Standard::PHILIPS;
        bool masterClockOut = false;
        DataFrameFormat dataFrameFormat = DataFrameFormat::BITS16;
        BitDepth bitDepth = BitDepth::BITS16;
        ClockPolarity clockPolarity = ClockPolarity::LOW;
    };

    namespace detail
    {
        template<std::uint8_t deviceId>
        constexpr gpio::AltFn getAlternateFunction()
        {
            if constexpr (deviceId < 2)
                return gpio::AltFn::SPI1_2;
            else
                return gpio::AltFn::SPI3;
        }

        constexpr std::uint32_t computeClockDivider(
            bool masterClockOutEnable, 
            std::uint32_t packetLength,
            std::uint32_t i2sClockFrequency,
            std::uint32_t sampleFrequency)
        {
            // MCKL_OUT == ON  => FS = i2sclk / (256 * (2*I2SDIV + ODD))
            // MCKL_OUT == OFF => FS = i2sclk / (2*packetLength * (2*I2SDIV + ODD))
            if (masterClockOutEnable)
            {
                return (((i2sClockFrequency / 256U) * 10U) / sampleFrequency + 5U) / 10U;
            }
            else
            {
                return (((i2sClockFrequency / (2U * packetLength)) * 10U) / sampleFrequency + 5U) / 10U;
            }
        }

        template<class SpiX, 
            std::uint32_t i2sClockFrequency_, 
            std::uint32_t sampleFrequency_, 
            bool use16BitData_, 
            bool mcklOut_>
        void configureI2sClock()
        {
            constexpr std::uint32_t packetLength = use16BitData_ ? 16U : 32U;
            constexpr std::uint32_t clockDivider = computeClockDivider(mcklOut_, packetLength, i2sClockFrequency_, sampleFrequency_);
            static_assert((clockDivider > 2U), "The clock frequency for the I2S peripheral is too low");

            constexpr std::uint32_t i2sodd = (uint32_t)(clockDivider & (uint32_t)1U);
            constexpr std::uint32_t i2sdiv = (uint32_t)((clockDivider - i2sodd) / 2U);

            reg::apply(SpiX{},
                reg::write(board::spi::I2SPR::ODD, constant_c<i2sodd>),
                reg::write(board::spi::I2SPR::I2SDIV, constant_c<i2sdiv>),
                reg::write(board::spi::I2SPR::MCKOE, constant_c<mcklOut_>));
        }
    }

    template<MasterConfig config>
    struct makeI2sMaster_t
    {
        template<class Board>
        constexpr auto operator()(Board boardDescriptor) const
        {
            constexpr auto spiX = boardDescriptor.getPeripheral(PeripheralTypes::SPI<config.id>); 
            using board::spi::I2SCFGR::I2sCfgVal;
            using SpiX = decltype(spiX);

            // Configure pins
            constexpr gpio::AltFn alternateFunction = detail::getAlternateFunction<config.id>();
            gpio::makeAltFnPin<gpio::AltFnPinConfig {
                .pin = config.serialClockPin,
                .altFn = alternateFunction
            }>(boardDescriptor);
            gpio::makeAltFnPin<gpio::AltFnPinConfig {
                .pin = config.wordSelectPin,
                .altFn = alternateFunction
            }>(boardDescriptor);
            gpio::makeAltFnPin<gpio::AltFnPinConfig {
                .pin = config.serialDataPin,
                .altFn = alternateFunction
            }>(boardDescriptor);
            
            if constexpr (config.masterClockOut)
            {
                gpio::makeAltFnPin<gpio::AltFnPinConfig {
                    .pin = config.masterClockPin,
                    .altFn = alternateFunction
                }>(boardDescriptor);
            }

            // Configure peripheral
            spiX.enable();
            detail::configureI2sClock<
                SpiX, 
                round(boardDescriptor.getI2SClockFrequency()), 
                config.sampleRate, 
                config.bitDepth == BitDepth::BITS16, 
                config.masterClockOut>();
                
            constexpr I2sCfgVal i2cConfig = config.transferMode == TransferMode::RX_ONLY ?
                I2sCfgVal::MASTER_RX : I2sCfgVal::MASTER_TX;

            reg::apply(SpiX{},
                reg::write(board::spi::I2SCFGR::CKPOL, constant_c<config.clockPolarity>),
                reg::write(board::spi::I2SCFGR::I2SSTD, constant_c<config.standard>),
                reg::write(board::spi::I2SCFGR::I2SCFG, constant_c<i2cConfig>),
                reg::write(board::spi::I2SCFGR::CHLEN, constant_c<config.bitDepth>),
                reg::write(board::spi::I2SCFGR::DATLEN, constant_c<config.dataFrameFormat>),
                // Set i2s mode instead of Spi
                reg::set(board::spi::I2SCFGR::I2SMOD));

            // Disable DMA
            reg::apply(SpiX{}, 
                reg::clear(board::spi::CR2::TXDMAEN),
                reg::clear(board::spi::CR2::RXDMAEN));

            // Enable
            reg::set(SpiX{}, board::spi::I2SCFGR::I2SE);  

            // Enable interrupt 
            auto interrupt = spi::detail::getInterrupt<config.id>();
            boardDescriptor.enableIRQ(interrupt);

            return I2S<SpiX>{ boardDescriptor.getInterruptEvent(interrupt) };   
        }
    };

    template<MasterConfig config>
    inline constexpr makeI2sMaster_t<config> makeI2sMaster {};
}
