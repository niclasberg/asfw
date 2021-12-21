#include "../catch.hpp"
#include "drivers/i2s.hpp"
#include "drivers/gpio.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "../mocks/mock_board.hpp"
#include <ratio>

using MockSpi = MockPeripheral<board::spi::tag>;
using MockGpio = MockPeripheral<board::gpio::tag>;

namespace {
    async::Event spiInterrupt;

    struct MockPeripherals
    {
        constexpr MockSpi getPeripheral(PeripheralTypes::tags::Spi<0>) const { return {}; }
        constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }

        async::EventEmitter getInterruptEvent(decltype(board::Interrupts::SPI1)) { return {&spiInterrupt}; }
    };
}

TEST_CASE("I2S")
{
    using namespace drivers;
    auto mockSpi = MockSpi{};
    resetPeripheral(mockSpi);
    auto mockBoard = makeMockBoard(is2ClockFrequency<48'000'000>, mockPeripherals<MockPeripherals>);

    SECTION("Master init default options")
    {
        i2s::makeI2sMaster(mockBoard, deviceId<0>, i2s::sampleFreq<48000>, 
            i2s::masterClockPin<0, 0>, i2s::serialDataPin<0, 0>, i2s::serialClockPin<0, 0>, i2s::wordSelectPin<0, 0>);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SE) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::CHLEN) == 0);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::CKPOL) == 0);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::DATLEN) == 0);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SSTD) == 0);    // Philips standard
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SMOD) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SCFG) == 2);    // Master transmit
        REQUIRE(reg::read(mockSpi, board::spi::I2SPR::MCKOE) == 0);
        //REQUIRE(reg::read(mockSpi, board::spi::I2SPR::I2SDIV) == 0);
    }

    SECTION("Master init non-default options")
    {
        i2s::makeI2sMaster(mockBoard, deviceId<0>, 
            i2s::sampleFreq<48000>,
            i2s::BitDepth::BITS32, 
            i2s::DataFrameFormat::BITS24,
            i2s::ClkPolarity::HIGH,
            i2s::Standard::MSB,
            i2s::MasterClockOut::ON,
            i2s::masterClockPin<0, 0>, i2s::serialDataPin<0, 0>, i2s::serialClockPin<0, 0>, i2s::wordSelectPin<0, 0>);

        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SE) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::CHLEN) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::CKPOL) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::DATLEN) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SSTD) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SMOD) == 1);
        REQUIRE(reg::read(mockSpi, board::spi::I2SCFGR::I2SCFG) == 2);    // Master transmit
        REQUIRE(reg::read(mockSpi, board::spi::I2SPR::MCKOE) == 1);
    }

    /*SECTION("Master blocking write, 2 bytes")
    {
        auto i2sDev = i2s::makeMaster(mockSpi, i2s::sampleFreq<48000>);
        i2sDev.init(mockBoard);

        // Set TXE bit (otherwise the call write call will block)
        setRegisterBit(mockSpi, board::spi::SR::TXE);

        uint16_t value = 0x1da3;
        i2sDev.write(&value, 1);

        REQUIRE(reg::read(mockSpi, board::spi::DR::DR) == value);
    }*/
}