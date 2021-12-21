#pragma once
#include "field_location.hpp"
#include "field.hpp"

namespace reg
{
    // Temporary storage for a register value
    template<class _Location> struct RegisterValue;

    template<class T, class _Owner, class _Offset>
    struct RegisterValue<FieldLocation<T, _Owner, _Offset>>
    {
        explicit RegisterValue(T value) : value(value) {}
        T value;
    };
}