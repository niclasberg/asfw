#include "../catch.hpp"
#include <async/stop_token.hpp>

TEST_CASE("Unstoppable token")
{
    SECTION("Should fullfill the StopToken concept")
    {
        STATIC_REQUIRE(async::StopToken<async::UnstoppableToken>);
    }
}