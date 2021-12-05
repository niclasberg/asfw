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

        auto op = connect(
            useState(5, [](int & value) { 
                return justValue(value);
            }),
            receiveValue([&](auto value) {
                valueReceived = value;
            }));
        async::start(op);

        REQUIRE(valueReceived == 5);
    }
}