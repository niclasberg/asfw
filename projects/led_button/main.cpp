#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/gpio.hpp>
#include <async/receive.hpp>
#include <async/connect.hpp>

namespace clock = board::clock;
using namespace drivers;

int main()
{
    auto boardDescriptor = board::makeBoard(
        clock::makeClockConfig());
    
    auto led = gpio::makeOutputPin(boardDescriptor, PIN<3, 12>);
    auto button = gpio::makeInputInterruptPin(boardDescriptor, PIN<0, 0>, gpio::Interrupt::RISING_EDGE);

    auto operation = async::connect(
        button.whenChanged(),
        async::receiveNext([&led](bool value) {
            led.toggle();
        }));
        
    operation.start();

    for(;;) 
    {
        
    }
}