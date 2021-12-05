#pragma once
#include "sender.hpp"

namespace async
{
    namespace detail
    {
        struct BindOperation
        {

        };
    }

    /**
     * Bind a SenderFactory to an EventSource. Whenever
     * an event is emitted, the SenderFactory will be invoked
     * and the created sender will be started.
     */
    inline constexpr struct BindType
    {
        template<ManySender S1, class F>
        auto operator()(S1 && source, F && senderFactory) const
        {

        }
    } bind;
}