#include "../catch.hpp"
#include <async/sequence.hpp>
#include <async/just.hpp>
#include <async/receive.hpp>
#include <async/use_state.hpp>
#include <async/map.hpp>
#include <async/execute_sync.hpp>
#include <async/outcome.hpp>
#include <iostream>

TEST_CASE("Sequence")
{
    SECTION("Should be able to send the value produced by the last sender")
    {
        auto sender = async::sequence(
            async::just(20),
            async::just(),
            async::just(10));

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), int, void>);
        REQUIRE(outcome == async::makeSuccess<void>(10));
    }

    SECTION("Should be possible to sequence sequences")
    {
        auto sender = async::sequence(async::sequence(async::just(10)));

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), int, void>);
        REQUIRE(outcome == async::makeSuccess<void>(10));
    }

    SECTION("Should emit the first error that occurs")
    {
        auto sender = async::sequence(
            async::just(),
            async::justError(double(10)),
            async::justError(double(20)),
            async::just());

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, double>);
        REQUIRE(outcome == async::makeError<void>(double(10)));
    }

    SECTION("Should short-circuit on done")
    {
        bool lastWasCalled = false;

        auto sender = async::sequence(
            async::just(),
            async::justDone(),
            async::just() | async::map([&]() { lastWasCalled = true; }));

        auto outcome = async::executeSync(std::move(sender));

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(outcome == async::makeDone<void, void>());
        REQUIRE(!lastWasCalled);
    }

    SECTION("Sequence should work together with useState()")
    {
        int out1 = 0;
        int out2 = 0;
        auto sender = async::sequence(
            async::useState(50, [&](auto & state) {
                return async::just()
                | async::map([&]() {
                    out1 = state;
                });
            }),
            async::useState(100, [&](auto & state) {
                return async::just()
                | async::map([&]() {
                    out2 = state;
                });
            }));
        auto op = async::connect(std::move(sender), async::receiveValue([]() { }));
        async::start(op);
        
        REQUIRE(out1 == 50);
        REQUIRE(out2 == 100);
    }
}