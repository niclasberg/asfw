#pragma once
#include "field_location.hpp"

namespace reg
{
    inline constexpr struct uncheckedRead_t
    {
        template<class F, class T, class _Owner, class _Offset>
        INLINE decltype(auto) operator()(F && handler, FieldLocation<T, _Owner, _Offset> location) const
        {
            return static_cast<F&&>(handler).read(location);
        }
    } uncheckedRead {};
}