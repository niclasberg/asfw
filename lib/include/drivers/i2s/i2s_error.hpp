#pragma once

namespace drivers::i2s
{
    enum class I2sError
    {
        BUSY,
        UNDERRUN,
        FRAME_ERROR
    };
}