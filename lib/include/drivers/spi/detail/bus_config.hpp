#pragma once

namespace drivers::spi::detail
{
    enum class BusConfig 
    {
        FULL_DUPLEX,
        HALF_DUPLEX,
        READ_ONLY,
        WRITE_ONLY
    };
}