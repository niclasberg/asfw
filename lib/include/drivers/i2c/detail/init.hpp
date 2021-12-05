#pragma once
#include "types.hpp"
#include "reg/apply.hpp"
#include "reg/write.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX, std::uint32_t fPClk, std::uint32_t f>
    void initClocks(I2cX & i2cX, uint32_<fPClk>, uint32_<f>) 
    { 
        if constexpr (f <= 100000)
        {
            // Initialize in standard mode
            constexpr auto ccr = uint32_c<fPClk / (2 * f)>;
            constexpr auto trise = uint32_c<fPClk / 1000000 + 1>;

            // Enable standard mode i2c
            reg::apply(i2cX, 
                reg::write(board::i2c::CCR::F_S, uint32_c<0>),
                reg::write(board::i2c::CCR::CCR, ccr));
            reg::write(i2cX, board::i2c::TRISE::TRISE, trise);
        }
        else
        {
            static_assert(f == -1, "Fast mode is not supported by this driver");
        }
    }
}