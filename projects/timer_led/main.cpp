#include <drivers/gpio.hpp>
#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <schedulers/cooperative_scheduler.hpp>
#include <async/event.hpp>
#include <async/every.hpp>
#include <async/transform.hpp>
#include <async/execute_sync.hpp>

using namespace drivers;
using namespace schedulers;

int main()
{
    auto boardDescriptor = board::makeBoard(board::clock::makeClockConfig());
    auto scheduler = makeCooperativeScheduler(boardDescriptor);
    auto led = gpio::makeOutputPin(boardDescriptor, PIN<3, 12>);

    auto task = 
        async::every(500)
        | async::transform([&]() {
            led.toggle();
        });

    async::executeSync(scheduler, std::move(task));
}