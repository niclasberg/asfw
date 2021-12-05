#pragma once
#include <cstdint>
#include "reg/peripheral_operations.hpp"
#include "board/regmap/i2c.hpp"
#include "reg/unchecked_read.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    void clearAddressBit()
    {
        // Clear address flag by reading SR1 followed by SR2
        std::uint32_t dummy = reg::uncheckedRead(I2cX{}, board::i2c::SR1::_Offset{});
        dummy = reg::uncheckedRead(I2cX{}, board::i2c::SR2::_Offset{});
        (void)dummy;
    }

    template<class I2cX, bool restartAfterComplete>
    void sendStopOrRestartCondition()
    {
        if constexpr (restartAfterComplete)
        {
            reg::set(I2cX{}, board::i2c::CR1::START);
        }
        else
        {
            reg::set(I2cX{}, board::i2c::CR1::STOP);
        }
    }
}