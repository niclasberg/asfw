#pragma once
#include "peripheral.hpp"
#include "register_value.hpp"
#include "read.hpp"
#include "write.hpp"

namespace reg
{
    template<class Src, class Location>
    INLINE auto getRegisterValue(Src src, Location)
    {
        return RegisterValue<Location>{src.read(Location{})};
    }

    template<class Tag, class Memory, class _Control, 
        class Location, class Mask, class RWPolicy, class ValueType>
    INLINE auto regHasValue(
        Peripheral<Tag, Memory, _Control> peripheral,
        Field<Location, Mask, RWPolicy, ValueType> field, 
        typename Field<Location, Mask, RWPolicy, ValueType>::Value value)
    {
        return read(peripheral, field) == detail::getValue(value, std::is_enum<ValueType>{});
    }

    template<class Tag, class Memory, class _Control, 
        class Location, class Mask, class RWPolicy, class ValueType>
    INLINE bool bitIsSet(
        Peripheral<Tag, Memory, _Control> peripheral, 
        Field<Location, Mask, RWPolicy, ValueType>)
    {
        static_assert(detail::isReadable<RWPolicy>::value, "Field is not readable");
        static_assert(detail::isSingleBit<Mask>::value, "Only single bit fields can be tested");
        return peripheral.read(Location{}) & (1 << Mask::shift);
    }
}