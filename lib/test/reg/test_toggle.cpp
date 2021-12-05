#include "../catch.hpp"
#include "reg/toggle.hpp"
#include "reg/bitmask.hpp"
#include "tmp/type_list.hpp"

namespace 
{
    class mock_tag {};
    template<std::uint32_t offset>
    using Offset = reg::FieldLocation<std::uint32_t, mock_tag, reg::FieldOffset<std::uint32_t, offset>>;

    template<std::uint32_t offset, std::uint32_t bit>
    constexpr auto singleBitField = reg::RWField<Offset<offset>, reg::BitMask32<bit, 1>>{};   
}

TEST_CASE("Register toggle", "[register]")
{
    SECTION("Should have an empty ValueTypes typedef")
    {
        using Type = decltype(reg::toggle(singleBitField<0, 0>));
        using ValueTypeList = typename Type::template ValueTypes<tmp::TypeList>;
        
        STATIC_REQUIRE(std::is_same_v<ValueTypeList, tmp::TypeList<>>);
    }

    SECTION("Toggle bit0")
    {
        constexpr auto toggleBit0 = reg::toggle(singleBitField<0, 0>);
        toggleBit0([](auto val) { REQUIRE(val == 0xfffffffe); }, 0xffffffff);
        toggleBit0([](auto val) { REQUIRE(val == 0x1); },        0x0);
        toggleBit0([](auto val) { REQUIRE(val == 0x0); },        0x1);
    }

    SECTION("Toggle bit5")
    {
        constexpr auto toggleBit5 = reg::toggle(singleBitField<10, 5>);
        toggleBit5([](auto val) { REQUIRE(val == 0x20); }, 0);
        toggleBit5([](auto val) { REQUIRE(val == 0x0); },  0x20);
    }
    
    SECTION("Toggle bit31")
    {
        constexpr auto toggleBit31 = reg::toggle(singleBitField<0, 31>);
        toggleBit31([](auto val) { REQUIRE(val == (1U << 31U)); }, 0);
        toggleBit31([](auto val) { REQUIRE(val == 0x7fffffffU); }, 0xffffffff);
    }
}