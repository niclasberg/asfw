#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/uart.hpp>
#include <drivers/gpio.hpp>
#include <async/receive.hpp>
#include <async/then.hpp>
#include <async/transform.hpp>
#include <cstring>
#include "async/just.hpp"

namespace clock = board::clock;
using namespace drivers;

int main()
{
    auto boardDescriptor = board::makeBoard(
        clock::makeClockConfig());
    
    auto serial = uart::makeUart(
        boardDescriptor,
        deviceId<3>,
        uart::baudRate<9600>,
        uart::txPin<2, 10>,
        uart::rxPin<2, 11>);
    auto led = gpio::makeOutputPin(boardDescriptor, PIN<3, 12>);

    const char * data = "Babushka I love you!!\r\n";
    const std::uint8_t * buffer = reinterpret_cast<const std::uint8_t *>(data);
    std::uint32_t len = std::strlen(data);

    using namespace async;
    auto sender = 
        serial.write(buffer, len) 
        | transform([]() { return 10; })
        | then([&serial, buffer, len](auto) { return serial.write(buffer, len); });

    auto operation = connect(
        std::move(sender),
        receiveValue([&led]() {
            led.toggle();
        }));
        
    operation.start();

    for(;;) 
    {
        
    }
}