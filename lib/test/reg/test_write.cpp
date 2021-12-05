#include "../catch.hpp"
#include "reg/field.hpp"
#include "reg/write.hpp"

namespace 
{
    class mock_tag {};
    template<std::uint32_t offset>
    using Offset = reg::FieldLocation<std::uint32_t, mock_tag, reg::FieldOffset<std::uint32_t, offset>>;

    template<std::uint32_t offset, std::uint32_t bit, std::uint32_t size>
    constexpr auto field = reg::RWField<Offset<offset>, reg::BitMask32<bit, size>>{};
}

TEST_CASE("Register write")
{
    SECTION("Write litteral")
    {
        // Zero out bits
        reg::write(field<0, 0, 32>, uint32_c<0x00>)([](auto val) { REQUIRE(val == 0x00); }, 0xffff'ffff);
        reg::write(field<0, 8, 8>, uint32_c<0x00>) ([](auto val) { REQUIRE(val == 0xffff00ff); }, 0xffff'ffff);
        reg::write(field<0, 31, 1>, uint32_c<0x00>)([](auto val) { REQUIRE(val == 0x7fffffff); }, 0xffff'ffff);

        // Set some bits
        reg::write(field<0, 8, 8>, uint32_c<0xFF>)([](auto val) { REQUIRE(val == 0xff00); }, 0x0);
    }

    SECTION("Write runtime value")
    {
        // Zero out bits
        uint32_t zero = 0x00;
        reg::write(field<0, 0, 32>, zero)([](auto val) { REQUIRE(val == 0x00); }, 0xffff'ffff);
        reg::write(field<0, 8, 8>, zero) ([](auto val) { REQUIRE(val == 0xffff00ff); }, 0xffff'ffff);
        reg::write(field<0, 31, 1>, zero)([](auto val) { REQUIRE(val == 0x7fffffff); }, 0xffff'ffff);

        // Set some bits
        uint32_t ff = 0xff;
        reg::write(field<0, 8, 8>, ff)([](auto val) { REQUIRE(val == 0xff00); }, 0x0);
    }
}