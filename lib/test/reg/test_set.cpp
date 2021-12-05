#include "../catch.hpp"
#include "reg/set.hpp"
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

TEST_CASE("Register set", "[register]")
{
    SECTION("Should have an empty ValueTypes typedef")
    {
        using Type = decltype(reg::set(singleBitField<0, 0>));
        using ValueTypeList = typename Type::template ValueTypes<tmp::TypeList>;
        
        STATIC_REQUIRE(std::is_same_v<ValueTypeList, tmp::TypeList<>>);
    }

    SECTION("Set bit0")
    {
        constexpr auto setBit0 = reg::set(singleBitField<0, 0>);
        setBit0([](auto val) { REQUIRE(val == 0xffffffff); }, 0xffffffff);
        setBit0([](auto val) { REQUIRE(val == 0x1); },        0x0);
        setBit0([](auto val) { REQUIRE(val == 0x1); },        0x1);
    }

    SECTION("Set bit5")
    {
        constexpr auto setBit5 = reg::set(singleBitField<10, 5>);
        setBit5([](auto val) { REQUIRE(val == 0xffffffff); }, 0xffffffff);
        setBit5([](auto val) { REQUIRE(val == 0x20); },       0x0);
        setBit5([](auto val) { REQUIRE(val == 0x20); },       0x20);
    }

    SECTION("Set bit31")
    {
        constexpr auto setBit31 = reg::set(singleBitField<0, 31>);
        setBit31([](auto val) { REQUIRE(val == 0xffffffff); }, 0xffffffff);
        setBit31([](auto val) { REQUIRE(val == 0x80000000); }, 0x0);
        setBit31([](auto val) { REQUIRE(val == 0x80000000); }, 0x80000000);
    }
}