#pragma once

namespace drivers::dma
{
    enum class DmaSignal
    {
        TRANSFER_COMPLETE,
        TRANSFER_COMPLETE_MEMORY1,
        TRANSFER_HALF_COMPLETE,
        TRANSFER_HALF_COMPLETE_MEMORY1,
        TRANSFER_ERROR,
        FIFO_ERROR,
        DIRECT_MODE_ERROR
    };
}