#include "../catch.hpp"
#include <async/when_all.hpp>
#include <async/just.hpp>
#include <async/receive.hpp>
#include <async/map.hpp>
#include <async/execute_sync.hpp>
#include <async/outcome.hpp>

TEST_CASE("whenAll")
{
    SECTION("Should merge all non-void value types")
    {
        auto sender = async::whenAll(
            async::just(int(10)),
            async::just(),
            async::just(double(10)));

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<int, double>, void>);
        REQUIRE(outcome == async::makeSuccess<void>(std::tuple{int(10), double(10)}));
    }

    SECTION("Can execute a single eagerly sender")
    {
        auto sender = async::whenAll(async::just());

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<>, void>);
        REQUIRE(outcome == async::makeSuccess<void>(std::tuple<>{}));
    }

    SECTION("Can stack multiple whenAll senders")
    {
        auto sender = async::whenAll(async::whenAll(async::just(int(10))));

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<std::tuple<int>>, void>);
        REQUIRE(outcome == async::makeSuccess<void>(std::tuple<std::tuple<int>>{std::tuple{int(10)}}));
    }

    SECTION("Should execute multiple senders")
    {
        std::vector<bool> executed(3, false);
        auto sender = async::whenAll(
            async::just() | async::map([&]() { executed[0] = true; }),
            async::just() | async::map([&]() { executed[1] = true; }),
            async::just() | async::map([&]() { executed[2] = true; }));
        
        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<>, void>);
        REQUIRE(executed[0]);
        REQUIRE(executed[1]);
        REQUIRE(executed[2]);
        REQUIRE(outcome == async::makeSuccess<void>(std::tuple<>{}));
    }

    SECTION("Should call setError on the receiver when one of the senders completed with error")
    {
        auto sender = async::whenAll(
            async::just(),
            async::justError(int(10)),
            async::just());
        
        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<>, int>);
        REQUIRE(outcome == async::makeError<std::tuple<>>(int(10)));
    }

    SECTION("Should call setDone on the receiver when one of the senders completed with setDone")
    {
        auto sender = async::whenAll(
            async::just(),
            async::justDone(),
            async::just());
        
        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), std::tuple<>, void>);
        REQUIRE(outcome == async::makeDone<std::tuple<>, void>());
    }
}