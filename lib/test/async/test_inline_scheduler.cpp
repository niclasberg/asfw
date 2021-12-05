#include "../catch.hpp"
#include "async/inline_scheduler.hpp"

TEST_CASE("Inline scheduler")
{
    SECTION("Should fullfill the scheduler concept")
    {
        STATIC_REQUIRE(async::Scheduler<async::InlineScheduler>);
    }
}