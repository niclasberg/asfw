#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/vl6180.hpp>
#include <drivers/i2c.hpp>
#include <drivers/uart.hpp>
#include <schedulers/cooperative_scheduler.hpp>

#include <async/execute_sync.hpp>
#include <async/repeat.hpp>
#include <async/transform.hpp>

#include <cstring>

using namespace drivers;

int main()
{
    auto boardDescriptor = board::makeBoard(
        board::clock::makeClockConfig());
    auto scheduler = schedulers::makeCooperativeScheduler(boardDescriptor);
    
    auto serial = uart::makeUart(
        boardDescriptor,
        deviceId<3>,
        uart::baudRate<9'600>,
        uart::txPin<2, 10>,
        uart::rxPin<2, 11>);
    
    // I2C3 SCL on PA8, SDA on PC9
    auto rangeFinderI2c = i2c::makeI2CMaster(
        boardDescriptor,
        deviceId<2>, 
        i2c::serialClockFrequency<100'000>,
        i2c::clockPin<0, 8>,
        i2c::dataPin<2, 9>);

    Vl6180 rangeFinder{rangeFinderI2c};
    char msgBuffer[128];

    auto outcome = async::executeSync(scheduler,
        async::sequence(
            rangeFinder.init(),

            rangeFinder.getDeviceIdentification()
            | async::then([&](auto deviceId) {
                std::sprintf(msgBuffer, "Model id: %u, rev. %u.%u\r\n",
                    deviceId.modelId,
                    deviceId.modelRevision.major,
                    deviceId.modelRevision.minor);

                return serial.write((const uint8_t *)msgBuffer, std::strlen(msgBuffer));
            }),

            async::repeat(
                rangeFinder.readRange()
                | async::then([&](auto value) {
                    std::sprintf(msgBuffer, "Range : %#04x \r\n", value);
                    return serial.write((const uint8_t *)msgBuffer, std::strlen(msgBuffer));
                }))
        ));

    for (;;) { }
}