#pragma once
#include "drivers/spi/detail/write.hpp"

namespace drivers::i2s::detail
{
    // The i2s functionality is handled by the spi peripheral,
    // and writing is handled the same
    struct WriteHooks
    {
        // No hooks to inject here...
    };

    template<class SpiX, class DataType, async::VoidReceiver R>
    using WriteOperation = spi::detail::WriteOperation<SpiX, DataType, R, WriteHooks>;
}