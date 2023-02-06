#include "../catch.hpp"
#include <async/when_any.hpp>
#include <async/just.hpp>
#include <async/execute_sync.hpp>
#include <async/outcome.hpp>

TEST_CASE("WhenAny")
{
    SECTION("Should ")
    {
        auto sender = async::whenAny(async::just(int(10)));

        auto outcome = async::executeSync(std::move(sender));

        REQUIRE(outcome == async::makeSuccess<void>(std::variant<int>{int(10)}));
        STATIC_REQUIRE(async::Future<decltype(sender), std::variant<int>, void>);
    }
}