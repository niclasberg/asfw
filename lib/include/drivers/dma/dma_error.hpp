#pragma once

namespace drivers::dma
{
    enum class DmaError
    {
        BUSY,
        TRANSFER_ERROR,
        FIFO_ERROR,
        DIRECT_MODE_ERROR
    };
}