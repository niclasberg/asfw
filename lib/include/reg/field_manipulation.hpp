#pragma once
#include "field.hpp"

namespace detail
{
    template<class T, int index, int maskOffset, int locationOffset>
    struct OffsetField
    {
        static_assert(!std::is_same<T, T>::value, "Index is out of range");
    };

    template<class T, class _Owner, class TOffset, TOffset offset,
        std::uint8_t shift, std::uint8_t size, class _RWPolicy, class ValueType,
        int maskOffset, int locationOffset>
    struct OffsetField<
        Field<
            FieldLocation<T, _Owner, FieldOffset<TOffset, offset>>,
            BitMask<T, shift, size>,
            _RWPolicy, 
            ValueType>,
        0, maskOffset, locationOffset>
    {
        static constexpr int size = 1;
        constexpr auto operator()() const 
        {
            return hana::type_c<
                Field<
                    FieldLocation<T, _Owner, FieldOffset<TOffset, offset + locationOffset>>,
                    BitMask<T, shift + maskOffset, size>, 
                    _RWPolicy, ValueType>>;
        }
    };
}

template<class Parent, int ... offsets>
struct RepeatMask
{
    template<class T, T index>
    constexpr auto operator[](integral_constant<T, index>) const
    {
        constexpr auto offsetField = OffsetField<RepeatMask<Parent, offsets...>, index, 0, 0>{};
        return typename decltype(offsetField())::type{};
    }
};

namespace detail
{
    template<class Parent, int ... offsets, int index, int maskOffset, int locationOffset>
    struct OffsetField<RepeatMask<Parent, offsets...>, index, maskOffset, locationOffset>
    {
        static constexpr int size = sizeof...(offsets);
        constexpr auto operator()()
        {
            constexpr int offsetValues[] = {offsets...};
            constexpr auto currentIndex = int_<index> / int_<Parent::size>;
            constexpr auto subIndex = int_<index> - currentIndex * int_<Parent::size>;
            return hana::type_c<OffsetField<Parent, hana::value(subIndex), maskOffset + offsetValues[currentIndex], locationOffset>>;
        }
    };
}

template<class Parent, int ... offsets>
struct RepeatLocation
{

};