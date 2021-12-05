#include "../catch.hpp"
#include <async/sequence.hpp>
#include <async/just.hpp>
#include <async/receive.hpp>
#include <async/use_state.hpp>
#include <async/transform.hpp>
#include <iostream>

TEST_CASE("Sequence")
{
    SECTION("Should be able to send the value produced by the last sender")
    {
        int receivedValue = 0;

        auto op = async::connect(
            async::sequence(
                async::justValue(),
                async::justValue(),
                async::justValue(10)),
            async::receiveValue(
                [&](auto i) {
                    receivedValue = i;
                }));
        async::start(op);

        REQUIRE(receivedValue == 10);
    }

    SECTION("Should contain unique error_types from all senders")
    {
        auto sender = async::sequence(
            async::justError(int()),
            async::justError(double()),
            async::justError(float()),
            async::justError(double()),
            async::justError(int()));

        STATIC_REQUIRE(std::is_same_v<async::SenderErrorTypes<decltype(sender), tmp::TypeList>, tmp::TypeList<int, double, float>>);
    }

    SECTION("Sequence should work together with useState()")
    {
        int out1 = 0;
        int out2 = 0;
        auto sender = async::sequence(
            async::useState(50, [&](auto & state) {
                return async::justValue()
                | async::transform([&]() {
                    out1 = state;
                });
            }),
            async::useState(100, [&](auto & state) {
                return async::justValue()
                | async::transform([&]() {
                    out2 = state;
                });
            }));
        auto op = async::connect(std::move(sender), async::receiveValue([]() { }));
        async::start(op);
        
        REQUIRE(out1 == 50);
        REQUIRE(out2 == 100);
    }
}