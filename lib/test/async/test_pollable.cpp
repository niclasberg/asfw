#include "../catch.hpp"
#include "async/pollable.hpp"

TEST_CASE("Pollable")
{
    SECTION("Map")
    {
        STATIC_REQUIRE(
            async::waitFor(
                async::map(async::makeValue(15), [](int a) { return a+2; })) 
            == 
            async::makeSuccess<void>(17)); 
            
        STATIC_REQUIRE(
            async::waitFor(
                async::map(async::EmptyPollable{}, []() { return 10; }))
            == 
            async::makeSuccess<void>(10)
        );

        STATIC_REQUIRE(
            async::waitFor(
                async::map(async::ErrorPollable<int>{10}, []() { return 'b'; }))
            == 
            async::makeError<char>(10)
        );
    }

    SECTION("Map to void should evaluate side effects")
    {
        bool wasCalled = false;
        auto f = async::map(async::makeValue(15), [&](int) { wasCalled = true; });

        auto outcome = async::waitFor(std::move(f));

        REQUIRE(outcome == async::makeSuccess<void>()); 
        REQUIRE(wasCalled); 
        STATIC_REQUIRE(async::Pollable<decltype(f), void, void>);
    }

    SECTION("FlatMap")
    {
        STATIC_REQUIRE(
            async::waitFor(
                async::flatMap(async::makeValue(15), [](int a) { 
                    return async::makeValue(char(a+2)); 
                })
            )
            == 
            async::makeSuccess<void>(char(17))
        );

        STATIC_REQUIRE(
            async::waitFor(
                async::flatMap(async::makeValue(15), [](int) { 
                    return async::EmptyPollable{}; 
                })
            )
            == 
            async::makeSuccess<void>()
        );

        STATIC_REQUIRE(
            async::waitFor(
                async::flatMap(async::EmptyPollable{}, []() { 
                    return async::makeValue(15); 
                })
            )
            == 
            async::makeSuccess<void>(15)
        );
    }
}