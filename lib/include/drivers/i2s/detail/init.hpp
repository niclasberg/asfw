#pragma once
#include "types.hpp"
#include "board/regmap/spi.hpp"
#include "reg/apply.hpp"
#include "reg/write.hpp"

namespace drivers::i2s::detail
{
    using board::spi::I2SCFGR::I2sStdVal;
    using board::spi::I2SCFGR::ChLenVal;
    using board::spi::I2SCFGR::DataLenVal;
    using board::spi::I2SCFGR::CkPolVal;
    using board::spi::I2SCFGR::I2sCfgVal;

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

    template<class SpiX, std::uint32_t i2sClockFrequency_, std::uint32_t sampleFrequency_, bool use16BitData_, bool mcklOut_>
    void configureI2sClock(
        SpiX, 
        uint32_<i2sClockFrequency_>,
        uint32_<sampleFrequency_>,
        bool_<use16BitData_>,
        bool_<mcklOut_> mcklOut)
    {
        constexpr std::uint32_t packetLength = use16BitData_ ? 16U : 32U;
        constexpr std::uint32_t clockDivider = computeClockDivider(mcklOut_, packetLength, i2sClockFrequency_, sampleFrequency_);
        static_assert((clockDivider > 2U), "The clock frequency for the I2S peripheral is too low");

        const uint32_t i2sodd = (uint32_t)(clockDivider & (uint32_t)1U);
        const uint32_t i2sdiv = (uint32_t)((clockDivider - i2sodd) / 2U);

        reg::apply(SpiX{},
            reg::write(board::spi::I2SPR::ODD, i2sodd),
            reg::write(board::spi::I2SPR::I2SDIV, i2sdiv),
            reg::write(board::spi::I2SPR::MCKOE, mcklOut));
    }

    template<
        class SpiX,
        CkPolVal clockPolarity,
        I2sStdVal i2sStandard, 
        I2sCfgVal i2sMode, 
        bool _bitDepth, 
        DataLenVal frameFormat>
    void initI2S(
        SpiX,
        constant_<clockPolarity>,
        constant_<i2sStandard>,
        constant_<i2sMode>,
        bool_<_bitDepth>,
        constant_<frameFormat>)
    {
        reg::apply(SpiX{},
            reg::write(board::spi::I2SCFGR::CKPOL, constant_c<clockPolarity>),
            reg::write(board::spi::I2SCFGR::I2SSTD, constant_c<i2sStandard>),
            reg::write(board::spi::I2SCFGR::I2SCFG, constant_c<i2sMode>),
            reg::write(board::spi::I2SCFGR::CHLEN, bool_c<_bitDepth>),
            reg::write(board::spi::I2SCFGR::DATLEN, constant_c<frameFormat>),
            // Set i2s mode instead of Spi
            reg::set(board::spi::I2SCFGR::I2SMOD));

        // Enable
        reg::set(SpiX{}, board::spi::I2SCFGR::I2SE);  
    }
}