#include "../catch.hpp"
#include "async/just.hpp"
#include "async/then.hpp"
#include "async/receive.hpp"
#include <async/use_state.hpp>
#include <async/transform.hpp>

TEST_CASE("Bind")
{
    SECTION("Should work for a void sender")
    {
        char receivedValue = 0;
        auto sender = 
            async::justValue() | 
            async::then([] 
            { 
                return async::justValue('a'); 
            });

        auto op = async::connect(
            std::move(sender),
            async::receiveValue([&](char c) {
                receivedValue = c;
            }));
        async::start(op);

        REQUIRE(receivedValue == 'a');
    }

    SECTION("Should work for single argument sender")
    {
        char receivedValue = 0;
        auto sender = 
            async::justValue(10) | 
            async::then([](int) 
            { 
                return async::justValue('a'); 
            });

        auto op = async::connect(
            std::move(sender),
            async::receiveValue([&](char c) {
                receivedValue = c;
            }));
        async::start(op);

        REQUIRE(receivedValue == 'a');
    }

    SECTION("Should work together with useState()")
    {
        int out1 = 0;
        int out2 = 0;
        auto sender = 
            async::useState(50, [&](auto & state) {
                return async::justValue()
                | async::transform([&]() {
                    out1 = state;
                });
            })
            | async::then([&]() {
                return async::useState(100, [&](auto & state) {
                    return async::justValue()
                    | async::transform([&]() {
                        out2 = state;
                    });
                });
            });
        auto op = async::connect(std::move(sender), async::receiveValue([]() { }));
        async::start(op);
        
        REQUIRE(out1 == 50);
        REQUIRE(out2 == 100);
    }
}