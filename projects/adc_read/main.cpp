#include <drivers/adc.hpp>
#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/dma.hpp>
#include <drivers/uart.hpp>
#include <schedulers/cooperative_scheduler.hpp>

#include <cstring>
#include <async/execute_sync.hpp>
#include <async/repeat.hpp>
#include <async/then.hpp>
#include <async/just.hpp>

using namespace drivers;
using namespace schedulers;

int main()
{
    auto boardDescriptor = board::makeBoard(
        board::clock::makeClockConfig());
    auto scheduler = makeCooperativeScheduler(boardDescriptor);

    auto keyInput = adc::makeAdc(
        boardDescriptor,
        deviceId<0>,
        adc::analogPins(
            PIN<2, 1>,
            PIN<2, 2>, PIN<2, 3>,
            PIN<0, 0>, PIN<0, 1>, 
            PIN<0, 2>, PIN<0, 3>, 
            PIN<0, 4>, PIN<0, 5>, 
            PIN<0, 7>,
            PIN<2, 4>, PIN<2, 5>));

    auto keyInputDma = dma::makeStream(
        boardDescriptor,
        dma::streamId<1, 0>,
        dma::Channel::CHANNEL0,
        dma::DataSize::BITS16);

    auto serial = uart::makeUart(
        boardDescriptor,
        deviceId<3>,
        uart::baudRate<9600>,
        uart::txPin<2, 10>,
        uart::rxPin<2, 11>);

    std::uint16_t data[12];
    char msgBuffer[128];

    async::executeSync(scheduler,
        async::repeat(
            keyInput.read(keyInputDma, data)
            | async::then([&]() {
                std::sprintf(msgBuffer, "value: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x\r\n",
                        data[0], data[1], data[2], data[3], data[4], data[5], data[6], 
                        data[7], data[8], data[9], data[10], data[11]);
                return serial.write((const uint8_t *)msgBuffer, std::strlen(msgBuffer));
            })));
    
    for (;;) { }
}