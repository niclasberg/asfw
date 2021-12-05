#pragma once
#include "sender.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class R, class F>
        struct DeferOperation
        {
            void start()
            {
                func_();
            }

            F func_;
        };

    }
}