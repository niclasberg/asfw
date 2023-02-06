#include "../catch.hpp"
#include "async/and_then.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include <async/use_state.hpp>
#include <async/map.hpp>
#include <async/execute_sync.hpp>
#include <async/outcome.hpp>

TEST_CASE("Bind")
{
    SECTION("Should work for a void sender")
    {
        auto sender = async::just() 
            | async::andThen([] { return async::just('a'); });

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), char, void>);
        REQUIRE(outcome == async::makeSuccess<void>('a'));
    }

    SECTION("Should work for a void sender mapped to empty")
    {
        bool receivedValue = false;
        auto sender = async::just() 
            | async::andThen([] { return async::just(); });

        auto op = async::connect(
            std::move(sender), async::receiveValue([&]() { receivedValue = true; })
        );
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(receivedValue);
    }

    SECTION("Should work for single argument sender")
    {
        char receivedValue = 0;
        auto sender = 
            async::just(10) 
            | async::andThen([](int i) { return async::just(char(i)); });

        auto op = async::connect(
            std::move(sender),
            async::receiveValue([&](char c) {
                receivedValue = c;
            }));
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(sender), char, void>);
        REQUIRE(receivedValue == 10);
    }

    SECTION("Should work together with useState()")
    {
        int out1 = 0;
        int out2 = 0;
        auto sender = 
            async::useState(50, [&](auto & state) {
                return async::just()
                    | async::map([&]() { out1 = state; });
            })
            | async::andThen([&]() {
                return async::useState(100, [&](auto & state) {
                    return async::just()
                        | async::map([&]() { out2 = state; });
                });
            });
        auto op = async::connect(std::move(sender), async::receiveValue([]() { }));
        async::start(op);
        
        REQUIRE(out1 == 50);
        REQUIRE(out2 == 100);
    }
}