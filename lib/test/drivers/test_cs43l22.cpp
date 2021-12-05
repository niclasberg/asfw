#include "../catch.hpp"
#include "drivers/cs43l22.hpp"
#include "../mocks/mock_i2c.hpp"
#include <async/receive.hpp>

TEST_CASE("Cs43l22")
{
    MockI2c mockI2c;

    SECTION("Test1")
    {
        bool valueReceived = false;
        drivers::Cs43l22<MockI2c> device{mockI2c};

        auto op = async::connect(
            device.init(),
            async::receiveValue([&]() {
                valueReceived = true;
            }));
        async::start(op);

        REQUIRE(valueReceived);
    }
}