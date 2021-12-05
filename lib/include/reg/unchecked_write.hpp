#pragma once
#include "field_location.hpp"

namespace reg
{
    inline constexpr struct uncheckedWrite_t
    {
        template<class F, class T, class _Owner, class _Offset, class Value>
        INLINE decltype(auto) operator()(F && handler, FieldLocation<T, _Owner, _Offset> location, Value value) const
        {
            return static_cast<F&&>(handler).write(location, value);
        }
    } uncheckedWrite {};
}