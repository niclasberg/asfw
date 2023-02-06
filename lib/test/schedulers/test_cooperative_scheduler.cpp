#include "../catch.hpp"
#include "async/scheduler.hpp"
#include "schedulers/cooperative_scheduler.hpp"
#include "../mocks/mock_board.hpp"

namespace {
    async::Event interruptEvent;

    struct Peripherals
    {
        async::EventEmitter getInterruptEvent(decltype(board::Interrupts::SysTick)) { return {&interruptEvent}; }
    };
}

TEST_CASE("Cooperative scheduler")
{
    using namespace schedulers;
    auto mockBoard = MockBoard<Peripherals>();

    SECTION("Should fullfill the scheduler concept")
    {
        STATIC_REQUIRE(async::Scheduler<
            CooperativeScheduler<1, 1, typename decltype(mockBoard)::InterruptController>>);
    }

    SECTION("Should be able to run enqueued jobs")
    {
        auto scheduler = makeCooperativeScheduler(mockBoard);
        bool wasCalled = false;
        auto act = [&]() { wasCalled = true; };

        scheduler.post({&act});
        scheduler.poll();

        REQUIRE(wasCalled);
    }

    SECTION("Should be able to run delayed jobs")
    {
        auto scheduler = makeCooperativeScheduler(mockBoard);
        bool wasCalled = false;
        auto act = [&]() -> int { wasCalled = true; return -1;};

        scheduler.postAfter(1, {&act});

        // Timer has not yet reached 1ms, should not be called
        scheduler.poll();
        REQUIRE(wasCalled == false);

        interruptEvent.raise(); // Tick
        scheduler.poll();
        REQUIRE(wasCalled == true);
    }
}