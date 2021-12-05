#include "../catch.hpp"
#include "reg/combine.hpp"
#include "reg/field_location.hpp"
#include <utility>

namespace
{
    class mock_tag {};
    template<std::uint32_t offset>
    using Offset = reg::FieldLocation<std::uint32_t, mock_tag, reg::FieldOffset<std::uint32_t, offset>>;

    template<std::uint32_t valueToWrite>
    struct MockWrite
    {
        template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
        static inline constexpr bool isWrite = true;

        template<class F, class ... Results>
        constexpr decltype(auto) operator()(F && f, std::uint32_t, Results... results) const
        {
            return static_cast<F&&>(f)(valueToWrite, results...);
        }
    };

    template<class Value>
    struct MockRead
    {
        template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
        static inline constexpr bool isWrite = false;

        template<class F, class ... Results>
        constexpr decltype(auto) operator()(F && f, std::uint32_t value, Results... results) const
        {
            return static_cast<F&&>(f)(value, results..., valueToRead);
        }

        Value valueToRead;
    };
}

TEST_CASE("Combine actions")
{
    constexpr auto location = Offset<0>{};

    SECTION("One write")
    {
        auto combined = reg::combine(reg::detail::makeFieldAction(location, MockWrite<5>{}));

        combined([](auto val) { REQUIRE(val == 5); }, 10U);
    }

    SECTION("Two writes")
    {
        auto combined = reg::combine(
            reg::detail::makeFieldAction(location, MockWrite<5>{}),
            reg::detail::makeFieldAction(location, MockWrite<10>{}));

        combined([](auto val) { REQUIRE(val == 10); }, 10U);
    }

    SECTION("Read")
    {
        auto combined = reg::combine(reg::detail::makeFieldAction(location, MockRead<int>{5}));

        combined([](auto val, auto readValue) { REQUIRE(val == 10); REQUIRE(readValue == 5); }, 10U);
    }

    SECTION("Two reads and transform")
    {
        auto combined = reg::combine(
            reg::detail::makeFieldAction(location, MockRead<int>{5}),
            reg::detail::makeFieldAction(location, MockRead<bool>{false}));

        auto values = combined(
            [](auto, auto intValue, auto boolValue) {
                return std::make_pair(intValue, boolValue);
            },
            10U);

        REQUIRE(values.first == 5);
        REQUIRE(values.second == false);
    }
}