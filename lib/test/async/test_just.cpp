#include "../catch.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include "async/future.hpp"
#include "tmp/type_list.hpp"

TEST_CASE("JustValue")
{
    SECTION("Should correctly pass a void argument by value")
    {
        bool wasCalled = false;
        auto op = async::connect(
            async::just(),
            async::receiveValue(
                [&wasCalled]() {
                    wasCalled = true;
                }
            ));
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(async::just()), void, void>);
        REQUIRE(wasCalled);
    }

    SECTION("Should correctly pass a single argument by value")
    {
        int result = 0;
        auto sender = async::just(10);
        STATIC_REQUIRE(async::Future<decltype(sender), int, void>);
        
        auto op = async::connect(
            std::move(sender),
            async::receiveValue([&result](auto val) { result = val; })
        );
        async::start(op);

        REQUIRE(result == 10);
    }

    SECTION("JustError")
    {
        int result = 0;
        auto sender = async::justError(10);
        STATIC_REQUIRE(async::Future<decltype(sender), void, int>);
        
        auto op = async::connect(
            std::move(sender),
            async::receiveError([&result](auto val) { result = val; })
        );
        async::start(op);

        REQUIRE(result == 10);
    }

    SECTION("JustDone")
    {
        bool wasDone = false;
        auto sender = async::justDone();
        
        auto op = async::connect(
            std::move(sender),
            async::receiveDone([&wasDone]() { wasDone = true; })
        );
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(wasDone);
    }

    SECTION("Should accept lvalue argument")
    {
        int input = 10, result = 0;
        auto op = async::connect(
            async::just(input),
            async::receiveValue([&result](auto val) { result = val;})
        );
        async::start(op);

        REQUIRE(result == input);
    }
}