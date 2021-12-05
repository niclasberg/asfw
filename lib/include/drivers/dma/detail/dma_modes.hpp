#pragma once

namespace drivers::dma::detail
{
    enum class DmaMode 
    {
        NORMAL,
        CIRCULAR,
        DOUBLE_BUFFERED
    };
}