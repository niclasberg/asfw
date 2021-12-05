#include "../catch.hpp"
#include <async/when_all.hpp>
#include <async/just.hpp>
#include <async/receive.hpp>
#include <async/transform.hpp>

TEST_CASE("whenAll")
{
    SECTION("Can execute a single eagerly evaluated sender")
    {
        bool receivedValue = false;
        auto op = 
            async::connect(
                async::whenAll(async::justValue()),
                async::receiveValue([&]() {
                    receivedValue = true;
                }));
        async::start(op);

        REQUIRE(receivedValue);
    }

    SECTION("Should execute multiple senders")
    {
        bool receivedValue = false;
        std::vector<bool> executed(3, false);
        auto op = 
            async::connect(
                async::whenAll(
                    async::transform(
                        async::justValue(),
                        [&]() { executed[0] = true; }),
                    async::transform(
                        async::justValue(),
                        [&]() { executed[1] = true; }),
                    async::transform(
                        async::justValue(),
                        [&]() { executed[2] = true; })),
                async::receiveValue([&]() {
                    receivedValue = true;
                }));
        async::start(op);

        REQUIRE(executed[0]);
        REQUIRE(executed[1]);
        REQUIRE(executed[2]);
        REQUIRE(receivedValue);
    }

    SECTION("Should call setDone on the receiver when one of the senders completed with setDone")
    {
        bool receivedDone = false;
        auto op = 
            async::connect(
                async::whenAll(
                    async::justValue(),
                    async::justValue(),
                    async::justDone()),
                async::receiveDone([&]() {
                    receivedDone = true;
                }));
        async::start(op);

        REQUIRE(receivedDone);
    }
}