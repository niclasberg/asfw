#include "catch.hpp"
#include "rational.hpp"
#include <cstdint>

TEST_CASE("Rational")
{
    SECTION("Scalar-rational equality comparison operator")
    {
        REQUIRE(Rational<std::uint32_t>{0} == 0);
        REQUIRE(Rational<std::uint32_t>{std::numeric_limits<std::uint32_t>::max()} == std::numeric_limits<std::uint32_t>::max());
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFCU, 4} == 0x3FFF'FFFFU);

        REQUIRE(!(Rational<int>{-5} == 5));
        REQUIRE(!(Rational<std::uint32_t>{7, 3} == 2));
    }

    SECTION("Scalar-rational less than operator")
    {
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFEU} < 0xFFFF'FFFFU);
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFFU, 4} < 0xFFFF'FFFFU);
        REQUIRE(Rational<std::uint32_t>{0} < 1);
        REQUIRE(Rational<std::uint32_t>{89, 9} < 10);
        REQUIRE(!(Rational<std::uint32_t>{91, 9} < 10));
        REQUIRE(!(Rational<std::uint32_t>{1} < 0));
    }

    SECTION("Scalar-rational less than or equal operator")
    {
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFEU} <= 0xFFFF'FFFFU);
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFFU, 4} <= 0xFFFF'FFFFU);
        REQUIRE(Rational<std::uint32_t>{0xFFFF'FFFCU, 4} <= 0x3FFF'FFFFU);
        REQUIRE(Rational<std::uint32_t>{0} <= 1);
        REQUIRE(!(Rational<std::uint32_t>{1} <= 0));
    }

    SECTION("Division operator")
    {
        auto result = Rational<std::uint32_t>{10, 9} / 5;
        REQUIRE(result.num == 2);
        REQUIRE(result.den == 9);
    }
}