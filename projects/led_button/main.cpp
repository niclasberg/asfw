#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/gpio.hpp>
#include <async/receive.hpp>
#include <async/connect.hpp>

namespace clock = board::clock;
using namespace drivers;

// Pinout
inline constexpr auto LED_PIN = Pin(3, 12);
inline constexpr auto BUTTON_PIN = Pin(0, 0);

int main()
{
    auto boardDescriptor = board::makeBoard(
        clock::makeClockConfig());
    
    auto led = gpio::makeOutputPin<{ .pin = LED_PIN }>(boardDescriptor);
    auto button = gpio::makeInputInterruptPin<{
        .pin = BUTTON_PIN,
        .interrupt = gpio::Interrupt::RISING_EDGE
    }>(boardDescriptor);

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