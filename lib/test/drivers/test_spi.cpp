#include "../catch.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "../mocks/mock_board.hpp"
#include "async/receive.hpp"
#include "async/use_scheduler.hpp"
#include "async/inline_scheduler.hpp"
#include "board/regmap/spi.hpp"
#include "drivers/spi.hpp"

using MockSpi = MockPeripheral<board::spi::tag>;
template<class ... Ts> struct TypeList;

namespace {
    volatile async::Event spiInterruptEvent;
    struct MockPeripherals
    {
        constexpr MockSpi getPeripheral(PeripheralTypes::tags::Spi<0>) const { return {}; }
        constexpr MockSpi getPeripheral(PeripheralTypes::tags::Spi<1>) const { return {}; }
        constexpr MockSpi getPeripheral(PeripheralTypes::tags::Spi<2>) const { return {}; }

        async::EventEmitter getInterruptEvent(decltype(board::Interrupts::SPI1)) { return {&spiInterruptEvent}; }
    };
}

TEST_CASE("Spi make")
{
    auto mockBoard = makeMockBoard(mockPeripherals<MockPeripherals>);
    MockSpi mockSpi{};
    resetPeripheral(mockSpi);
    using namespace drivers;

    SECTION("Setting BitOrder::LSB_FIRST should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::BitOrder::LSB_FIRST);
        REQUIRE(reg::bitIsSet(mockSpi, board::spi::CR1::LSBFIRST));
    }

    SECTION("Setting BitOrder::MSB_FIRST should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::BitOrder::MSB_FIRST);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR1::LSBFIRST));
    }

    SECTION("Setting ClockPolarity::LOW should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::ClockPolarity::LOW);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR1::CPOL));
    }

    SECTION("Setting ClockPolarity::HIGH should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::ClockPolarity::HIGH);
        REQUIRE(reg::bitIsSet(mockSpi, board::spi::CR1::CPOL));
    }

    SECTION("Setting ClockPhase::LOW should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::ClockPhase::LOW);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR1::CPHA));
    }

    SECTION("Setting ClockPhase::HIGH should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::ClockPhase::HIGH);
        REQUIRE(reg::bitIsSet(mockSpi, board::spi::CR1::CPHA));
    }

    SECTION("Setting BaudRateDivider::DIV_2 should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::BaudRateDivider::DIV_2);
        REQUIRE(reg::read(mockSpi, board::spi::CR1::BR) == 0);
    }

    SECTION("Setting BaudRateDivider::DIV_256 should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::BaudRateDivider::DIV_256);
        REQUIRE(reg::read(mockSpi, board::spi::CR1::BR) == 0x7);
    }
    
    SECTION("Setting FrameFormat::BYTE should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::FrameFormat::BYTE);
        REQUIRE(reg::read(mockSpi, board::spi::CR1::DFF) == 0);
    }

    SECTION("Setting FrameFormat::HALF_WORD should set the correct register")
    {
        spi::makeSpi(mockBoard, deviceId<0>, spi::FrameFormat::HALF_WORD);
        REQUIRE(reg::read(mockSpi, board::spi::CR1::DFF) == 0x1);
    }
}

TEST_CASE("Spi read and write")
{
    using namespace drivers::spi;
    async::EventEmitter eventEmitter{&spiInterruptEvent};
    eventEmitter.unsubscribe();
    auto scheduler = async::InlineScheduler{};

    auto mockBoard = makeMockBoard(mockPeripherals<MockPeripherals>);
    MockSpi mockSpi{};
    resetPeripheral(mockSpi);
    auto spi = makeSpi(mockBoard, deviceId<0>);

    SECTION("The write sender should contain a value and error typedef")
    {
        using SenderType = decltype(spi.write(nullptr, 0));
        using ValueTypes = async::SenderValueTypes<SenderType, TypeList, TypeList>;
        using ErrorTypes = async::SenderErrorTypes<SenderType, TypeList>;

        STATIC_REQUIRE(std::is_same_v<ValueTypes, TypeList<TypeList<>>>);
        STATIC_REQUIRE(std::is_same_v<ErrorTypes, TypeList<SpiError>>);
    }

    SECTION("The write operation should finish instantaneously when the input is empty")
    {
        bool valueReceived = false;
        auto op = async::connect(
            spi.write(nullptr, 0),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() { valueReceived = true; })));

        op.start();
        REQUIRE(valueReceived);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR2::TXEIE));
    }

    SECTION("The write operation should set the TXIE register once started")
    {   
        std::uint8_t data[] = "hello";

        auto op = async::connect(
            spi.write(data, 5),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([]() { })));

        REQUIRE(!(reg::bitIsSet(mockSpi, board::spi::CR2::TXEIE)));
        op.start();
        REQUIRE(reg::bitIsSet(mockSpi, board::spi::CR2::TXEIE));
    }

    SECTION("The operation should write a single byte and then call setValue for a single byte input")
    {
        std::uint8_t data = 'a';
        bool valueReceived = false;

        auto op = async::connect(
            spi.write(&data, 1),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() { valueReceived = true; })));
        
        op.start();
        setRegisterBit(mockSpi, board::spi::SR::TXE); // Transmit buffer empty
        spiInterruptEvent.raise();
        spiInterruptEvent.raise();

        REQUIRE(getFieldValue(mockSpi, board::spi::DR::DR) == data);
        REQUIRE(valueReceived);
    }

    SECTION("Should push multiple bytes into the data register")
    {
        bool valueReceived = false;
        const std::uint8_t data[10] = {1, 4, 15, 0, 14, 123, 5, 8, 1, 55};
        int dataLen = 10;

        auto op = async::connect(
            spi.write(data, dataLen),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&]() { valueReceived = true; })));

        op.start();
        setRegisterBit(mockSpi, board::spi::SR::TXE); // Transmit buffer empty
        
        for (int i = 0; i < dataLen; ++i)
        {
            spiInterruptEvent.raise();
            REQUIRE(getFieldValue(mockSpi, board::spi::DR::DR) == data[i]);
        }
        spiInterruptEvent.raise();
        REQUIRE(valueReceived);
    }

    SECTION("The read sender should contain a value and error typedef")
    {
        using SenderType = decltype(spi.read(nullptr, 0));
        using ValueTypes = async::SenderValueTypes<SenderType, TypeList, TypeList>;
        using ErrorTypes = async::SenderErrorTypes<SenderType, TypeList>;

        STATIC_REQUIRE(std::is_same_v<ValueTypes, TypeList<TypeList<std::uint8_t *, std::uint32_t>>>);
        STATIC_REQUIRE(std::is_same_v<ErrorTypes, TypeList<SpiError>>);
    }

    SECTION("The read operation should finish instantaneously when the input is empty and pass the provided array and a size of zero")
    {
        std::uint8_t valueToSend = 42;
        bool valueReceived = false;
        std::uint8_t * dataReceived = nullptr;
        int sizeReceived = -1;

        auto op = async::connect(
            spi.read(&valueToSend, 0),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&](std::uint8_t * data, std::uint32_t size) { 
                    valueReceived = true; 
                    dataReceived = data;
                    sizeReceived = size;
                })));
        async::start(op);

        REQUIRE(valueReceived);
        REQUIRE(dataReceived == &valueToSend);
        REQUIRE(sizeReceived == 0);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR2::RXNEIE));
    }

    SECTION("The read operation should be able to read one byte")
    {
        std::uint8_t readBuffer[1] = {0};
        std::uint8_t * dataReceived = nullptr;
        int sizeReceived = -1;

        auto op = async::connect(
            spi.read(readBuffer, 1),
            async::addSchedulerToReceiver(
                scheduler,
                async::receiveValue([&](std::uint8_t * data, std::uint32_t size) { 
                    dataReceived = data;
                    sizeReceived = size;
                })));

        setFieldValue(mockSpi, board::spi::DR::DR, 0x24U);
        setRegisterBit(mockSpi, board::spi::SR::RXNE);
        async::start(op);

        spiInterruptEvent.raise();
        spiInterruptEvent.raise();

        REQUIRE(readBuffer[0] == 0x24);
        REQUIRE(sizeReceived == 1);
        REQUIRE(!reg::bitIsSet(mockSpi, board::spi::CR2::RXNEIE));
    }
}