#pragma once
#include "types.hpp"
#include "bitmask.hpp"
#include "field_location.hpp"

namespace reg
{
    template<class _Location, class _Mask, class _RWPolicy, class ValueType> struct Field;

    template<class T, class _Owner, class TOffset, TOffset offset,
        T shift, std::uint8_t size, class _RWPolicy, class ValueType>
    struct Field<
        FieldLocation<T, _Owner, FieldOffset<TOffset, offset>>,
        BitMask<T, shift, size>,
        _RWPolicy,
        ValueType>
    {
        constexpr Field() = default;

        using Value = ValueType;
        using Mask = BitMask<T, shift, size>;
        using Location = FieldLocation<T, _Owner, FieldOffset<TOffset, offset>>;
    };

    namespace detail 
    {
        struct RWPolicy {};
        struct ROPolicy {};
        struct WOPolicy {};

        template<class _RWPolicy> struct isWriteable : false_ {};
        template<> struct isWriteable<RWPolicy> : true_ {};
        template<> struct isWriteable<WOPolicy> : true_ {};

        template<class _RWPolicy> struct isReadable : false_ {};
        template<> struct isReadable<RWPolicy> : true_ {};
        template<> struct isReadable<ROPolicy> : true_ {};
    }

    template<class _Offset, class _Mask, class Type = typename _Offset::Value>
    using RWField = Field<_Offset, _Mask, detail::RWPolicy, Type>;

    template<class _Offset, class _Mask, class Type = typename _Offset::Value>
    using ROField = Field<_Offset, _Mask, detail::ROPolicy, Type>;

    template<class _Offset, class _Mask, class Type = typename _Offset::Value>
    using WOField = Field<_Offset, _Mask, detail::WOPolicy, Type>;
}