#include "../catch.hpp"
#include "async/emit.hpp"
#include "async/fold.hpp"
#include "async/execute_sync.hpp"

TEST_CASE("Emit")
{
    SECTION("asdf")
    {
        auto stream = async::emit(10);
        
        auto outcome = async::executeSync(
            std::move(stream) | 
            async::foldInplace(std::vector<int>{}, [](auto & acc, auto && current) {
                acc.emplace_back(current);
            }));
        
        REQUIRE(outcome == async::makeSuccess<void>(std::vector<int>{10}));
        STATIC_REQUIRE(async::Stream<decltype(stream), int, void>);
    }
}