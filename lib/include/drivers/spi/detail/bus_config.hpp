#pragma once

namespace drivers::spi::detail
{
    enum class BusConfig 
    {
        FullDuplex,
        HalfDuplex,
        RxOnly,
        TxOnly
    };
}