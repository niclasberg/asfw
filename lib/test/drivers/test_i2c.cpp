#include "../catch.hpp"
#include "drivers/i2c.hpp"
#include "drivers/i2c/i2c_like.hpp"
#include "../mocks/mock_board.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "async/receive.hpp"
#include "async/use_scheduler.hpp"
#include "async/inline_scheduler.hpp"

using MockI2C = MockPeripheral<board::i2c::tag>;
using MockGpio = MockPeripheral<board::gpio::tag>;

namespace {
    async::Event eventInterrupt;
    async::Event errorInterrupt;
}

struct MockPeripherals
{
    constexpr MockI2C getPeripheral(PeripheralTypes::tags::I2c<0>) const { return {}; }
    constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }

    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::I2C1_EV)) { return {&eventInterrupt}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::I2C1_ER)) { return {&errorInterrupt}; }
};

void generateStartCondition()
{
    setRegisterBit(MockI2C{}, board::i2c::SR1::SB);
    eventInterrupt.raise();
    clearRegisterBit(MockI2C{}, board::i2c::SR1::SB);
}

void doAddressPhase()
{
    setRegisterBit(MockI2C{}, board::i2c::SR1::ADDR);
    eventInterrupt.raise();
    clearRegisterBit(MockI2C{}, board::i2c::SR1::ADDR);
}

void transferBufferEmpty()
{
    setRegisterBit(MockI2C{}, board::i2c::SR1::TxE);
    eventInterrupt.raise();
    clearRegisterBit(MockI2C{}, board::i2c::SR1::TxE);
}

void byteTransferFinished()
{
    setRegisterBit(MockI2C{}, board::i2c::SR1::BTF);
    eventInterrupt.raise();
    clearRegisterBit(MockI2C{}, board::i2c::SR1::BTF);
}

TEST_CASE("I2C")
{
    using namespace drivers;
    auto mockI2C = MockI2C{};
    resetPeripheral(mockI2C);
    MockBoard<MockPeripherals, MockClockConfig { .apb1ClockFrequency = 8000000 }> mockBoard;
    async::EventEmitter eventEmitter{&eventInterrupt};
    async::EventEmitter errorEmitter{&errorInterrupt};
    eventEmitter.unsubscribe();
    errorEmitter.unsubscribe();
    auto scheduler = async::InlineScheduler{};
    constexpr i2c::MasterConfig config
    {
        .deviceId = 0,
        .clockPin = Pin(0, 0), 
        .dataPin = Pin(0, 0),
        .serialClockFrequency = 100'000
    };

    SECTION("Init in standard mode")
    {
        i2c::makeI2c<config>(mockBoard);

        REQUIRE(isEnabled(mockI2C));
        REQUIRE(reg::read(mockI2C, board::i2c::CR2::FREQ) == 8);
        REQUIRE(reg::read(mockI2C, board::i2c::CCR::F_S) == 0); // Standard mode
        REQUIRE(reg::read(mockI2C, board::i2c::CCR::CCR) == 0x28);
        REQUIRE(reg::read(mockI2C, board::i2c::TRISE::TRISE) == 0x9);
        REQUIRE(reg::read(mockI2C, board::i2c::CR1::PE) == 1);
    }

    SECTION("Should fullfill the I2cLike concept")
    {
        auto dev = i2c::makeI2c<config>(mockBoard);
        
        STATIC_REQUIRE(i2c::I2cLike<decltype(dev)>);
    }

    SECTION("Write single byte")
    {
        bool valueReceived = false;
        std::uint8_t data = 0xF1;
        auto dev = i2c::makeI2c<config>(mockBoard);

        auto op = async::connect(
            dev.write(0x10, &data, 1),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() {
                    valueReceived = true;
                })));
        async::start(op);

        // Interrupts should be enabled
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITEVTEN));
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITERREN));

        generateStartCondition();

        // The slave address should have been written to DR
        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0x20);

        doAddressPhase();

        // TODO: Check if SR1 and SR2 was read

        transferBufferEmpty();
        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0xF1);
        
        // The transmission should not finish until BTF is set
        REQUIRE(!valueReceived);

        byteTransferFinished();

        REQUIRE(valueReceived);
    }

    SECTION("Read single byte")
    {
        bool valueReceived = false;
        auto dev = i2c::makeI2c<config>(mockBoard);
        std::uint8_t data = 0;

        auto op = async::connect(
            dev.read(0x10, &data, 1),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() {
                    valueReceived = true;
                })));
        async::start(op);

        // Interrupts should be enabled
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITEVTEN));
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITERREN));

        generateStartCondition();

        // The slave address should have been written to DR
        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0x21);

        doAddressPhase();
    }

    SECTION("Write and read single byte")
    {
        std::uint8_t writeBuffer[1] = {0xF4};
        std::uint8_t readBuffer[1] = {0};
        bool valueReceived = false;
        auto dev = i2c::makeI2c<config>(mockBoard);

        auto op = async::connect(
            dev.writeAndRead(0x10, writeBuffer, 1, readBuffer, 1),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() {
                    valueReceived = true;
                })));
        async::start(op);

        // Interrupts should be enabled
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITEVTEN));
        REQUIRE(reg::bitIsSet(mockI2C, board::i2c::CR2::ITERREN));

        generateStartCondition();

        // The slave address should have been written to DR
        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0x20);

        doAddressPhase();

        transferBufferEmpty();
        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0xF4);

        byteTransferFinished();

        generateStartCondition();

        REQUIRE(reg::read(mockI2C, board::i2c::DR::DR) == 0x21);

        doAddressPhase();
    }

    /*SECTION("Write single byte (interrupt)")
    {
        auto dev = i2c::makeI2CMaster(mockI2C, i2c::serialClockFrequency<100000>);
        dev.init(mockBoard);

        I2cSimulator<decltype(dev)> simulator(dev);
        
        simulator.start();
        bool isDone = false;
        i2c::Error res;
        std::uint8_t data = 0xF1;
        dev.write(0x10, &data, 1, [&isDone, &res](i2c::Error result) {
            isDone = true;
            res = result;
        });

        while( ! isDone) { }
        simulator.stop();

        REQUIRE(res == i2c::Error::NO_ERROR);
        REQUIRE(simulator.slaveAddr == 0x10);
        REQUIRE(simulator.isWrite());
        REQUIRE((simulator.data.size() == 1 && simulator.data[0] == data));
    }

    SECTION("Write multiple bytes (blocking)")
    {
        auto dev = i2c::makeI2CMaster(mockI2C, i2c::serialClockFrequency<100000>);
        dev.init(mockBoard);
        I2cSimulator<decltype(dev)> simulator(dev);

        simulator.start();
        std::vector<std::uint8_t> data{0xF1, 0x02, 0x33, 0x44, 0x13, 0x80};
        REQUIRE(dev.write(0x10, data.data(), (std::uint16_t) data.size()));
        simulator.stop();

        REQUIRE(simulator.slaveAddr == 0x10);
        REQUIRE(simulator.isWrite());
        REQUIRE(std::equal(simulator.data.begin(), simulator.data.end(), data.begin()));
    }

    SECTION("Write multiple bytes (interrupt)")
    {
        auto dev = i2c::makeI2CMaster(mockI2C, i2c::serialClockFrequency<100000>);
        dev.init(mockBoard);

        I2cSimulator<decltype(dev)> simulator(dev);
        
        simulator.start();
        bool isDone = false;
        i2c::Error res;
        std::vector<std::uint8_t> data{0xF1, 0x02, 0x33, 0x44, 0x13, 0x80};
        dev.write(
            0x14, 
            data.data(), 
            (std::uint16_t) data.size(), 
            [&isDone, &res](i2c::Error result) {
                isDone = true;
                res = result;
            });
        while( ! isDone) { }
        simulator.stop();

        REQUIRE(res == i2c::Error::NO_ERROR);
        REQUIRE(simulator.slaveAddr == 0x14);
        REQUIRE(simulator.isWrite());
        REQUIRE(std::equal(simulator.data.begin(), simulator.data.end(), data.begin()));
    }*/
}