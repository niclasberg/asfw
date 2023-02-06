#include "../catch.hpp"
#include "async/map.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include "async/execute_sync.hpp"
#include "tmp/type_list.hpp"

TEST_CASE("Map")
{
    SECTION("Value type should not change for an identity transform")
    {
        auto transformed = async::map(async::just(int(5)), std::identity{});
        STATIC_REQUIRE(std::is_same_v<async::future_value_t<decltype(transformed)>, int>);
    }

    SECTION("Transform should be able to transform void to non-void")
    {
        auto sender = async::map(async::just(), []() -> double { return 10.0; });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), double, void>);
        REQUIRE(outcome == async::makeSuccess<void>(10.0));
    }

    SECTION("Transform should be able to transform non-void to void")
    {
        auto sender = async::map(async::just(int(5)), [](int) { });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(outcome == async::makeSuccess<void>());
    }

    SECTION("Transform should be able to transform void to void")
    {
        auto sender = async::map(async::just(), []() { });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(outcome == async::makeSuccess<void>());
    }

    SECTION("Transform should be able to transform non-void to non-void")
    {
        auto sender = async::map(async::just(char(10)), [&](char c) { return int(5*c); });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), int, void>);
        REQUIRE(outcome == async::makeSuccess<void>(50));
    }

    SECTION("Should pass errors through un-transformed")
    {
        const int errorCode = 10;
        auto sender = async::map(async::justError(errorCode), []() { });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, int>);
        REQUIRE(outcome == async::makeError<void>(errorCode));
    }

    SECTION("Should pass done through un-transformed")
    {
        auto sender = async::map(async::justDone(), []() { });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(outcome == async::makeDone<void, void>());
    }

    SECTION("The factory should support piping")
    {
        auto sender = async::just(int(5)) 
            | async::map([&](int i) { return i*2; })
            | async::map([&](int i) { return double(i+1); });
        
        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), double, void>);
        REQUIRE(outcome == async::makeSuccess<void>(11.0));
    }
}