#pragma once
#include "scheduler.hpp"
#include "delegate.hpp"

namespace async
{
    struct InlineScheduler
    {
        bool post(Delegate<void(void)> action)
        {
            // Evaluate eagerly
            action();
            return true;
        }

        bool postFromISR(Delegate<void(void)> action)
        {
            action();
            return true;
        }

        void poll() { }
    };
}