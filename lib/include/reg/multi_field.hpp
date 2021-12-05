#pragma once
#include "field.hpp"
#include <boost/hana/tuple.hpp>

namespace reg
{
    template<std::uint8_t count, std::uint8_t stride> struct RepMask {};
    template<std::uint8_t count, std::uint8_t stride> struct RepLocation {};

    template<class _Location, class _Mask, class _RWPolicy, class _RepeatMask, class _RepeatLocation, class ValueType> 
    struct MultiField;

    template<class T, class _Owner, class TOffset, TOffset baseOffset,
        T maskShift, std::uint8_t maskSize, class _RWPolicy,
        std::uint8_t maskCount, std::uint8_t maskStride,
        std::uint8_t locationCount, std::uint8_t locationStride, class ValueType>
    struct MultiField<
        FieldLocation<T, _Owner, FieldOffset<TOffset, baseOffset>>, 
        BitMask<T, maskShift, maskSize>, 
        _RWPolicy, 
        RepMask<maskCount, maskStride>, 
        RepLocation<locationCount, locationStride>,
        ValueType>
    {
        constexpr MultiField() = default;

        template<class U, U indx>
        constexpr auto operator[](hana::integral_constant<U, indx>) const 
        {
            static_assert(indx < (maskCount*locationCount), "Bit mask index out of range");

            constexpr auto offset = integral_c<TOffset, baseOffset> + integral_c<TOffset, (indx / maskCount)> * integral_c<TOffset, locationStride>;
            constexpr auto mask = integral_c<T, maskShift> + integral_c<T, (indx % maskCount)> * integral_c<T, maskStride>;

            using Loc = FieldLocation<T, _Owner, FieldOffset<TOffset, hana::value(offset)>>;
            using Mask = BitMask<T, hana::value(mask), maskSize>;

            return Field<Loc, Mask, _RWPolicy, ValueType>{};
        }
    };

    template<class _Location, class _Mask, class _RepMask, class _RepLoc, class Type = typename _Location::Value>
    using RWMultiField = MultiField<_Location, _Mask, detail::RWPolicy, _RepMask, _RepLoc, Type>;

    template<class _Location, class _Mask, class _RepMask, class _RepLoc, class Type = typename _Location::Value>
    using ROMultiField = MultiField<_Location, _Mask, detail::ROPolicy, _RepMask, _RepLoc, Type>;

    template<class _Location, class _Mask, class _RepMask, class _RepLoc, class Type = typename _Location::Value>
    using WOMultiField = MultiField<_Location, _Mask, detail::WOPolicy, _RepMask, _RepLoc, Type>;   
}