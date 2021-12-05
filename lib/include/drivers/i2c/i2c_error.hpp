#pragma once

namespace drivers::i2c
{
    enum class I2cError
    {
        BUSY,
        ACKNOWLEDGE_FAILURE,
        ARBITRATION_LOST,
        BUS_ERROR,
        UNKNOWN
    };
}