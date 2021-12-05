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

    template<class Location, class Mask, class RWPolicy, class ValueType>
    INLINE bool bitIsSet(
        RegisterValue<Location> regValue, 
        Field<Location, Mask, RWPolicy, ValueType>)
    {
        static_assert(detail::isReadable<RWPolicy>::value, "Field is not readable");
        static_assert(detail::isSingleBit<Mask>::value, "Only single bit fields can be tested");
        return regValue.value & (1 << Mask::shift);
    }
}