#include "../catch.hpp"
#include "drivers/gpio.hpp"
#include "reg/peripheral_operations.hpp"
#include "peripheral_types.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "../mocks/mock_board.hpp"
#include "board/regmap/gpio.hpp"
#include <array>
#include <map>

#include "async/receive.hpp"

using MockGpio = MockPeripheral<board::gpio::tag>;
using MockExti = MockPeripheral<board::exti::tag>;
using MockSysCfg = MockPeripheral<board::syscfg::tag>;

namespace {
    async::Event gpioInterruptEvent;
}

struct Peripherals
{
    constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }
    constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<2>) const { return {}; }
    constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<4>) const { return {}; }
    constexpr MockExti getPeripheral(PeripheralTypes::tags::Exti) const { return {}; }
    constexpr MockSysCfg getPeripheral(PeripheralTypes::tags::SysCfg) const { return {}; }

    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::EXTI2)) { return {&gpioInterruptEvent}; }
};

using namespace hana::literals;

TEST_CASE("Gpio")
{
    using namespace drivers;
    MockGpio mockGpio{};
    MockExti mockExti{};
    MockSysCfg mockSysCfg{};
    resetPeripheral(mockGpio);
    resetPeripheral(mockExti);
    resetPeripheral(mockSysCfg);
    async::EventEmitter eventEmitter{&gpioInterruptEvent};
    eventEmitter.unsubscribe();
    auto mockBoard = makeMockBoard(mockPeripherals<Peripherals>);

    SECTION("Make output pin defaults")
    {        
        gpio::makeOutputPin(mockBoard, PIN<0, 10>);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[10_c]) == 1);
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[10_c]) == 0);
        REQUIRE(reg::read(mockGpio, board::gpio::OTYPER::OT[10_c]) == 0);
        REQUIRE(reg::read(mockGpio, board::gpio::OSPEEDR::OSPEEDR[10_c]) == 0);
        REQUIRE(isEnabled(mockGpio));
    }

    SECTION("Make output pin with options")
    {
        gpio::makeOutputPin(
            mockBoard, PIN<0, 5>, 
            gpio::OutType::OPEN_DRAIN, 
            gpio::PuPd::PULL_DOWN, 
            gpio::Speed::HIGH);
        
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[5_c]) == 2);
        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[5_c]) == 1);
        REQUIRE(reg::read(mockGpio, board::gpio::OTYPER::OT[5_c]) == 1);
        REQUIRE(reg::read(mockGpio, board::gpio::OSPEEDR::OSPEEDR[5_c]) == 0x2);
    }

    SECTION("Make input pin defaults")
    {
        gpio::makeInputPin(mockBoard, PIN<0, 14>);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[14_c]) == 0);
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[14_c]) == 0);
        REQUIRE(isEnabled(mockGpio));
    }

    SECTION("Input pin read")
    {
        auto pin = gpio::makeInputPin(mockBoard, PIN<0, 0>);
        fillDeviceMemory(mockGpio, std::numeric_limits<std::uint32_t>::max());
        REQUIRE(pin.read() == true);

        fillDeviceMemory(mockGpio, 0);
        REQUIRE(pin.read() == false);
    }

    SECTION("Init input pin with options")
    {
        gpio::makeInputPin(
            mockBoard, 
            PIN<0, 2>,
            gpio::PuPd::PULL_DOWN);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[2_c]) == 0);
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[2_c]) == 0x2);
    }

    SECTION("Init analog pin default")
    {
        gpio::makeAnalogPin(mockBoard, PIN<0, 12>);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[12_c]) == 3);
        REQUIRE(isEnabled(mockGpio));
    }

    /*SECTION("Init AltFn pin")
    {
        gpio::makeAltFnPin(mockBoard, PIN<0, 12>, gpio::AltFn::I2C);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[12_c]) == 2);
        REQUIRE(reg::read(mockGpio, board::gpio::AFR::AFR[12_c]) == 4);
        REQUIRE(isEnabled(mockGpio));
    }

    SECTION("Init AltFn pin with options")
    {
        gpio::makeAltFnPin(
            mockBoard, PIN<0, 4>,
            gpio::AltFn::SPI3,
            gpio::OutType::PUSH_PULL,
            gpio::PuPd::PULL_UP,
            gpio::Speed::VERY_HIGH,
            gpio::AltFn::I2C);

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[4_c]) == 2);
        REQUIRE(reg::read(mockGpio, board::gpio::AFR::AFR[4_c]) == 6);
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[4_c]) == 1);
    }*/

    SECTION("Init input pin with interrupt")
    {
        gpio::makeInputInterruptPin(
            mockBoard, PIN<4, 2>, 
            gpio::PuPd::PULL_DOWN,
            gpio::Interrupt::RISING_FALLING_EDGE);

        REQUIRE(isEnabled(mockGpio));
        REQUIRE(isEnabled(mockSysCfg));

        REQUIRE(reg::read(mockGpio, board::gpio::MODER::MODER[2_c]) == 0);
        REQUIRE(reg::read(mockGpio, board::gpio::PUPDR::PUPDR[2_c]) == 2);
        REQUIRE(reg::read(mockSysCfg, board::syscfg::EXTICR::EXTI[2_c]) == 4);
        REQUIRE(reg::read(mockExti, board::exti::FTSR::TR[2_c]) == 1);
        REQUIRE(reg::read(mockExti, board::exti::RTSR::TR[2_c]) == 1);
    }

    SECTION("Init input pin with interrupt and rising edge interrupt")
    {
        gpio::makeInputInterruptPin(
            mockBoard, PIN<0, 2>, 
            gpio::Interrupt::RISING_EDGE);

        REQUIRE(reg::read(mockExti, board::exti::FTSR::TR[2_c]) == 0);
        REQUIRE(reg::read(mockExti, board::exti::RTSR::TR[2_c]) == 1);
    }

    SECTION("Init input pin with interrupt and falling edge interrupt")
    {
        gpio::makeInputInterruptPin(
            mockBoard, PIN<0, 2>, 
            gpio::Interrupt::FALLING_EDGE);

        REQUIRE(reg::read(mockExti, board::exti::FTSR::TR[2_c]) == 1);
        REQUIRE(reg::read(mockExti, board::exti::RTSR::TR[2_c]) == 0);
    }

    SECTION("Interrupt read many")
    {
        bool currentValue = false;
        auto pin = gpio::makeInputInterruptPin(
            mockBoard, PIN<0, 2>, 
            gpio::Interrupt::FALLING_EDGE);
        
        auto op = async::connect(
            pin.whenChanged(),
            async::receiveSignal([&currentValue](bool value) {
                currentValue = value;
            }));
        
        // The interrupt should be masked before starting the operation
        REQUIRE(reg::read(mockExti, board::exti::IMR::MR[2_c]) == 0);

        async::start(op);
        // Ensure that the masking has been disabled
        REQUIRE(reg::read(mockExti, board::exti::IMR::MR[2_c]) == 1);
        
        setRegisterBit(mockGpio, board::gpio::IDR::IDR[uint8_c<2>]);
        gpioInterruptEvent.raise();
        REQUIRE(currentValue == true);

        clearRegisterBit(mockGpio, board::gpio::IDR::IDR[uint8_c<2>]);
        gpioInterruptEvent.raise();
        REQUIRE(currentValue == false);
    }

    /*SECTION("Interrupt read single")
    {
        bool currentValue = false;
        auto pin = gpio::makeInputInterruptPin(
            MockBoard{}, PIN<0, 2>, 
            gpio::Interrupt::FALLING_EDGE);
        
        auto op = async::connect(
            pin.awaitChange(),
            async::receiveValue([&currentValue](bool value) {
                currentValue = value;
            }));
        
        // The interrupt should be masked before starting the operation
        REQUIRE(reg::read(mockExti, board::exti::IMR::MR[2_c]) == 0);

        op.start();

        // Ensure that the masking has been disabled
        REQUIRE(reg::read(mockExti, board::exti::IMR::MR[2_c]) == 1);

        // Set pin high, and trigger interrupt
        setRegisterBit(mockGpio, board::gpio::IDR::IDR[uint8_c<2>]);
        triggerIrq(board::Interrupts::EXTI2);
        REQUIRE(currentValue == true);

        // The awaitChange method should unregister the interrupt and 
        // enable interrupt masking once a value has been received
        REQUIRE(interruptHandlers.find(board::Interrupts::EXTI2) == interruptHandlers.end());
        REQUIRE(reg::read(mockExti, board::exti::IMR::MR[2_c]) == 0);
    }*/
}