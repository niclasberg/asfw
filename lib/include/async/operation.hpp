#pragma once

namespace async
{
    inline constexpr struct start_t final
    {
        template<class Op>
        void operator()(Op & operation) const
        {
            operation.start();
        }
    } start{};

    template<class T>
    concept Operation = requires(T & operation) {
        { start(operation) };
    };

    inline constexpr struct next_t final
    {
        template<class Op>
        void operator()(Op & op) const
        {
            op.next();
        }
    } next {};

    template<class O>
    concept StreamOperation = requires(O & operation) {
        { start(operation) };
        { next(operation) };
    };
}