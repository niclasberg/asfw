#pragma once

namespace drivers::dma::detail
{
    enum class DmaMode 
    {
        SINGLE_SHOT,
        CIRCULAR,
        DOUBLE_BUFFERED
    };
}