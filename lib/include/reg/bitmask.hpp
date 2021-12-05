#pragma once
#include "types.hpp"

namespace reg
{
    template<class T, T _shift, std::uint8_t size>
    struct BitMask : integral_constant<T, ((1U << size) - 1) << _shift>
    {
        static_assert(8*sizeof(T) > (_shift + size - 1), "Invalid bit mask");
        static constexpr T shift = _shift;
    };

    // Boundary case: size == 8*sizeof(T)
    template<>
    struct BitMask<std::uint8_t, 0, 8> : integral_constant<std::uint8_t, 0xFF>
    {
        static constexpr std::uint8_t shift = 0;
    };

    template<>
    struct BitMask<std::uint16_t, 0, 16> : integral_constant<std::uint16_t, 0xFFFF>
    {
        static constexpr std::uint16_t shift = 0;
    };

    template<>
    struct BitMask<std::uint32_t, 0, 32> : integral_constant<std::uint32_t, 0xFFFFFFFF>
    {
        static constexpr std::uint32_t shift = 0;
    };

    template<std::uint8_t shift, std::uint8_t size>
    using BitMask8 = BitMask<std::uint8_t, shift, size>;

    template<std::uint16_t shift, std::uint16_t size>
    using BitMask16 = BitMask<std::uint16_t, shift, size>;

    template<std::uint32_t shift, std::uint32_t size>
    using BitMask32 = BitMask<std::uint32_t, shift, size>;

    namespace detail
    {
        template<class Mask> struct isSingleBit;

        template<class T, T shift, std::uint8_t size>
        struct isSingleBit<BitMask<T, shift, size>> : hana::false_ {};

        template<class T, T shift>
        struct isSingleBit<BitMask<T, shift, 1>> : hana::true_ {};   
    }   
}