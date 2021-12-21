#pragma once
#include "sender.hpp"
#include "receiver.hpp"

namespace async
{
    namespace detail
    {
        
    }

    inline constexpr struct doEffect_t final
    {
        template<Sender S, VoidSender TEffect>
        auto operator()(S && sender, TEffect && effectSender)
        {

        }

    } doEffect {};
}