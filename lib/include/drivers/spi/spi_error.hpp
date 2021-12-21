#pragma once

namespace drivers::spi
{
    enum class SpiError
    {
        BUSY,
        MODE_FAULT,
        OVERRUN,
        DMA_ERROR
    };
}