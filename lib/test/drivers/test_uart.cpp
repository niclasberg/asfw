#include "../catch.hpp"
#include "peripheral.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "../mocks/mock_board.hpp"
#include "drivers/uart.hpp"

using MockUart = MockPeripheral<board::uart::tag>;
using MockGpio = MockPeripheral<board::gpio::tag>;
using MockExti = MockPeripheral<board::exti::tag>;
using MockSysCfg = MockPeripheral<board::syscfg::tag>;

namespace {
    async::Event interruptEvent;
}

struct MockPeripherals
{
    constexpr MockUart getPeripheral(PeripheralTypes::tags::Uart<3>) const { return {}; }
    constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }
    constexpr MockExti getPeripheral(PeripheralTypes::tags::Exti) const { return {}; }
    constexpr MockSysCfg getPeripheral(PeripheralTypes::tags::SysCfg) const { return {}; }

    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::UART4)) { return {&interruptEvent}; }
};

TEST_CASE("Uart")
{
    using namespace drivers;
    resetPeripheral(MockUart{});

    /**
     * Baud rate config tests
     * 
     * Br (kbps)  | UARTDIV   | Mantissa | Fraction
     * ----------------------------------------------------------
     * pcklFrequency = 8Mhz:
     * 1.2 KBps   | 416.6875  | 416      | 11
     * 2.4 KBps   | 208.3125  | 208      | 5
     * 9.6 KBps   | 52.0625   | 52       | 1
     * 19.2 KBps  | 26.0625   | 26       | 1
     * 38.4 KBps  | 13        | 13       | 0
     * 57.6 KBps  | 8.6875    | 8        | 11
     * 115.2 KBps | 4.3125    | 4        | 5
     * 230.4 KBps | 2.1875    | 2        | 3
     * 460.8 KBps | 1.0625    | 1        | 1
     * 
     * pcklFrequency = 12Mhz:
     * 1.2 KBps   | 625       | 625      | 0
     * 2.4 KBps   | 312.5     | 312      | 8
     * 9.6 KBps   | 78.125    | 78       | 2
     * 19.2 KBps  | 39.0625   | 39       | 1
     * 38.4 KBps  | 19.5625   | 19       | 9
     * 57.6 KBps  | 13        | 13       | 0
     * 115.2 KBps | 6.5       | 6        | 8
     * 230.4 KBps | 3.25      | 3        | 4
     * 460.8 KBps | 1.625     | 1        | 10
     */
#define TEST_BAUD_RATE(PCKL, BAUD, MANTISSA, FRACTION) \
    SECTION("Test baud rate (pckl " #PCKL " Mhz, br " #BAUD " bps)") \
    { \
        auto mockBoard = makeMockBoard(apb1ClockFrequency<PCKL>, mockPeripherals<MockPeripherals>); \
        uart::makeUart(mockBoard, deviceId<3>, uart::baudRate<BAUD>, uart::txPin<0, 0>, uart::rxPin<0, 0>); \
        REQUIRE(reg::read(MockUart{}, board::uart::BRR::DIV_Mantissa) == MANTISSA); \
        REQUIRE(reg::read(MockUart{}, board::uart::BRR::DIV_Fraction) == FRACTION); \
    } 

    TEST_BAUD_RATE(8000000, 1200, 416, 11);
    TEST_BAUD_RATE(8000000, 2400, 208, 5);
    TEST_BAUD_RATE(8000000, 9600, 52, 1);
    TEST_BAUD_RATE(8000000, 19200, 26, 1);
    TEST_BAUD_RATE(8000000, 38400, 13, 0);
    TEST_BAUD_RATE(8000000, 57600, 8, 11);
    TEST_BAUD_RATE(8000000, 115200, 4, 5);
    TEST_BAUD_RATE(8000000, 230400, 2, 3);
    TEST_BAUD_RATE(8000000, 460800, 1, 1);
    TEST_BAUD_RATE(12000000, 1200, 625, 0);
    TEST_BAUD_RATE(12000000, 2400, 312, 8);
    TEST_BAUD_RATE(12000000, 9600, 78, 2);
    TEST_BAUD_RATE(12000000, 19200, 39, 1);
    TEST_BAUD_RATE(12000000, 38400, 19, 9);
    TEST_BAUD_RATE(12000000, 57600, 13, 0);
    TEST_BAUD_RATE(12000000, 115200, 6, 8);
    TEST_BAUD_RATE(12000000, 230400, 3, 4);
    TEST_BAUD_RATE(12000000, 460800, 1, 10);

#undef TEST_BAUD_RATE

#define TEST_FRAME_FORMAT(FORMAT, MVAL, PSVAL, PCEVAL, STOPVAL) \
    SECTION("Test data format: " #FORMAT ")") \
    { \
        auto mockBoard = makeMockBoard(mockPeripherals<MockPeripherals>, apb1ClockFrequency<2000000>); \
        uart::makeUart(mockBoard, deviceId<3>, uart::baudRate<9600>, uart::txPin<0, 0>, uart::rxPin<0, 0>, uart::FrameFormat::_##FORMAT); \
        REQUIRE(reg::read(MockUart{}, board::uart::CR1::M) == MVAL); \
        REQUIRE(reg::read(MockUart{}, board::uart::CR1::PS) == PSVAL); \
        REQUIRE(reg::read(MockUart{}, board::uart::CR1::PCE) == PCEVAL); \
        REQUIRE(reg::read(MockUart{}, board::uart::CR2::STOP) == STOPVAL); \
    }

    // Bits: 
    // PS (parity): 0 = Even, 1 = Odd
    // M (data bits): 0 = 8 bits, 1 = 9 bits
    // STOP: 0 = 1 bit, 2 = 2 bits
    TEST_FRAME_FORMAT(7E1, 0, 0, 1, 0);
    TEST_FRAME_FORMAT(7O1, 0, 1, 1, 0);
    TEST_FRAME_FORMAT(8N1, 0, 0, 0, 0);
    TEST_FRAME_FORMAT(8E1, 1, 0, 1, 0);
    TEST_FRAME_FORMAT(8O1, 1, 1, 1, 0);
    TEST_FRAME_FORMAT(7E2, 0, 0, 1, 2);
    TEST_FRAME_FORMAT(7O2, 0, 1, 1, 2);
    TEST_FRAME_FORMAT(8N2, 0, 0, 0, 2);
    TEST_FRAME_FORMAT(8E2, 1, 0, 1, 2);
    TEST_FRAME_FORMAT(8O2, 1, 1, 1, 2);

#undef TEST_FRAME_FORMAT
}