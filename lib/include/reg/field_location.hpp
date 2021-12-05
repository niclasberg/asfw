#pragma once
#include "types.hpp"

namespace reg
{
    template<class T, T offset>
    using FieldOffset = hana::integral_constant<T, offset>;

    template<class T, class Owner, class Offset> struct FieldLocation;

    template<class T, class _Owner, class TOffset, TOffset offset>
    struct FieldLocation<T, _Owner, FieldOffset<TOffset, offset>>
    {
        using Value = T;
        using Owner = _Owner;
        using Offset = FieldOffset<TOffset, offset>;
    };   
}