#include "../catch.hpp"
#include "reg/clear.hpp"
#include "tmp/type_list.hpp"

namespace 
{
    class mock_tag {};
    template<std::uint32_t offset>
    using Offset = reg::FieldLocation<std::uint32_t, mock_tag, reg::FieldOffset<std::uint32_t, offset>>;

    template<std::uint32_t offset, std::uint32_t bit>
    constexpr auto singleBitField = reg::RWField<Offset<offset>, reg::BitMask32<bit, 1>>{};   
}

TEST_CASE("Register clear", "[register]")
{
    SECTION("Should have an empty ValueTypes typedef")
    {
        using Type = decltype(reg::clear(singleBitField<0, 0>));
        using ValueTypeList = typename Type::template ValueTypes<tmp::TypeList>;
        
        STATIC_REQUIRE(std::is_same_v<ValueTypeList, tmp::TypeList<>>);
    }

    SECTION("Clear bit0")
    {
        constexpr auto clearBit0 = reg::clear(singleBitField<0, 0>);
        clearBit0([](auto val) { REQUIRE(val == 0xfffffffe); }, 0xffffffff);
        clearBit0([](auto val) { REQUIRE(val == 0x00); },       0x0);
        clearBit0([](auto val) { REQUIRE(val == 0x00); },       0x1);
    }

    SECTION("Clear bit5")
    {
        constexpr auto clearBit5 = reg::clear(singleBitField<10, 5>);
        clearBit5([](auto val) { REQUIRE(val == 0xffffffdf); }, 0xffffffff);
        clearBit5([](auto val) { REQUIRE(val == 0x00); },       0x00);
        clearBit5([](auto val) { REQUIRE(val == 0x00); },       0x20);
    }

    SECTION("Clear bit31")
    {
        constexpr auto clearBit31 = reg::clear(singleBitField<0, 31>);
        clearBit31([](auto val) { REQUIRE(val == 0x7fffffff); }, 0xffffffff);
        clearBit31([](auto val) { REQUIRE(val == 0x00); },       0x00);
        clearBit31([](auto val) { REQUIRE(val == 0x00); },       0x80000000);
    }
}