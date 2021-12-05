#include "../catch.hpp"
#include "async/inplace_stop_token.hpp"

TEST_CASE("InplaceStopSource")
{
    SECTION("InplaceStopToken should fullfill the StopToken concept")
    {
        STATIC_REQUIRE(async::StopToken<async::InplaceStopToken>);
    }

    SECTION("The stop token's state should change when stop is requested on a stopSource")
    {
        async::InplaceStopSource stopSource;
        auto token = stopSource.getToken();
        REQUIRE(!token.stopRequested());

        stopSource.requestStop();
        REQUIRE(token.stopRequested());
    }

    SECTION("A stop callback should be called when stop is requested")
    {
        bool callbackWasCalled = false;
        async::InplaceStopSource stopSource;
        auto token = stopSource.getToken();

        async::InplaceStopCallback stopCallback(token, [&]() {
            callbackWasCalled = true;
        });

        REQUIRE(callbackWasCalled == false);
        stopSource.requestStop();
        REQUIRE(callbackWasCalled == true);
    }
}