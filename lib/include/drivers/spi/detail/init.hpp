#pragma once
#include "types.hpp"
#include "bus_config.hpp"
#include "board/regmap/spi.hpp"
#include "reg/peripheral_operations.hpp"
#include "reg/apply.hpp"
#include "reg/write.hpp"

namespace drivers::spi
{
    namespace detail
    {
        using board::spi::CR1::CpolVal;
        using board::spi::CR1::CphaVal;
        using board::spi::CR1::BrVal;
        using board::spi::CR1::DffVal;        

        template<
            class SpiX, 
            CpolVal clkPolarity, 
            CphaVal clkPhase, 
            BrVal baudRateDivider, 
            DffVal dataFrameFormat, 
            BusConfig busConfig,
            bool isMaster, 
            bool lsbFirst>
        void initSpi(
            SpiX spiX, 
            constant_<clkPolarity>,
            constant_<clkPhase>,
            constant_<baudRateDivider>,
            constant_<dataFrameFormat>,
            constant_<busConfig>,
            bool_<isMaster>,
            bool_<lsbFirst>)
        {
            spiX.enable();

            reg::apply(spiX,
                reg::write(board::spi::CR1::BR, constant_c<baudRateDivider>),
                reg::write(board::spi::CR1::CPOL, constant_c<clkPolarity>),
                reg::write(board::spi::CR1::CPHA, constant_c<clkPhase>),
                reg::write(board::spi::CR1::DFF, constant_c<dataFrameFormat>),
                reg::write(board::spi::CR1::LSBFIRST, bool_c<lsbFirst>),
                reg::write(board::spi::CR1::MSTR, bool_c<isMaster>));

            // Figure out NSS pin handling and put here

            if constexpr (busConfig == BusConfig::FullDuplex)
            {
                
            }

            reg::write(SpiX{}, board::spi::CR1::SPE, true_c);
        }
    }
}