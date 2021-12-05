#pragma once
#include "types.hpp"

namespace board
{
    namespace adc
    {
        template<std::uint8_t portNo, std::uint8_t pinNo>
        struct AdcChannel
        {
            static_assert(portNo == -1, "The supplied gpio pin does not support analog");
        };

        template<> struct AdcChannel<0, 0> : uint8_<0> {};
        template<> struct AdcChannel<0, 1> : uint8_<1> {};
        template<> struct AdcChannel<0, 2> : uint8_<2> {};
        template<> struct AdcChannel<0, 3> : uint8_<3> {};
        template<> struct AdcChannel<0, 4> : uint8_<4> {};
        template<> struct AdcChannel<0, 5> : uint8_<5> {};
        template<> struct AdcChannel<0, 6> : uint8_<6> {};
        template<> struct AdcChannel<0, 7> : uint8_<7> {};
        template<> struct AdcChannel<1, 0> : uint8_<8> {};
        template<> struct AdcChannel<1, 1> : uint8_<9> {};
        template<> struct AdcChannel<2, 0> : uint8_<10> {};
        template<> struct AdcChannel<2, 1> : uint8_<11> {};
        template<> struct AdcChannel<2, 2> : uint8_<12> {};
        template<> struct AdcChannel<2, 3> : uint8_<13> {};
        template<> struct AdcChannel<2, 4> : uint8_<14> {};
        template<> struct AdcChannel<2, 5> : uint8_<15> {};
    }
}