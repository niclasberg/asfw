#include "../catch.hpp"
#include <async/use_state.hpp>
#include <async/just.hpp>
#include <async/receive.hpp>

using namespace async;

TEST_CASE("Use state")
{
    SECTION("asd")
    {
        int valueReceived = 0;
        auto sender = useState(5, [](int & value) { return just(value); });

        auto op = connect(std::move(sender), receiveValue([&](auto value) { valueReceived = value; }));
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(sender), int, void>);
        REQUIRE(valueReceived == 5);
    }

    SECTION("Should handle void type")
    {
        bool valueReceived = false;
        auto sender = useState(5, [](int &) { return just(); });

        auto op = connect(std::move(sender), receiveValue([&]() { valueReceived = true; }));
        async::start(op);

        STATIC_REQUIRE(async::Future<decltype(sender), void, void>);
        REQUIRE(valueReceived);
    }
}