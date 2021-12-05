#pragma once
#include "scheduler.hpp"
#include "sender.hpp"

namespace async
{
    inline constexpr struct on_t
    {
        template<class Scheduler, class Sender>
        auto operator()(Scheduler && scheduler, Sender && sender) const
        {
            
        }
    } on{};
}