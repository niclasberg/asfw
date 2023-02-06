#pragma once
#include "types.hpp"
#include "opt/option.hpp"
#include "board/interrupts.hpp"
#include <boost/hana/string.hpp>
#include "rational.hpp"
#include <vector>

namespace detail
{
    constexpr auto  AHB_FREQUENCY = hana::string_c<'A', 'H', 'B', 'F', 'R', 'E', 'Q'>;
    constexpr auto APB1_FREQUENCY = hana::string_c<'A', 'P', 'B', '1', 'F', 'R', 'E', 'Q'>;
    constexpr auto APB2_FREQUENCY = hana::string_c<'A', 'P', 'B', '2', 'F', 'R', 'E', 'Q'>;
    constexpr auto I2S_FREQUENCY = hana::string_c<'I', '2', 'S', 'F', 'R', 'E', 'Q'>;
    constexpr auto MOCK_PERIPHERALS = hana::string_c<'P', 'E', 'R', 'I'>;

    struct DefaultMockPeripherals {};
}

struct MockInterruptController
{
    static void enableIRQs() { }
    static void disableIRQs() { }
};

struct MockClockConfig
{
    std::uint32_t ahbClockFrequency = 0;
    std::uint32_t apb1ClockFrequency = 0;
    std::uint32_t apb2ClockFrequency = 0;
    std::uint32_t i2sClockFrequency = 0;
};

template<class MockPeripherals, MockClockConfig clockConfig = MockClockConfig{}>
class MockBoard : public MockPeripherals
{
    std::vector<bool> interruptIsEnabled;
public:
    using InterruptController = MockInterruptController;

    MockBoard() : interruptIsEnabled(128, false) { }

    constexpr Rational<std::uint32_t> getApb1ClockFrequency() const { return Rational{clockConfig.apb1ClockFrequency}; }
    constexpr Rational<std::uint32_t> getApb2ClockFrequency() const { return Rational{clockConfig.apb2ClockFrequency}; }
    constexpr Rational<std::uint32_t> getAhbClockFrequency() const { return Rational{clockConfig.ahbClockFrequency}; }
    constexpr Rational<std::uint32_t> getI2SClockFrequency() const { return Rational{clockConfig.i2sClockFrequency}; }

    template<int number>
    void enableIRQ(Interrupt<number>)
    {
        interruptIsEnabled[number] = true;
    }

    template<int number>
    void disableIRQ(Interrupt<number>)
    {
        interruptIsEnabled[number] = false;
    }

    template<std::uint32_t tickFrequency_>
    void enableSysTickIRQ(uint32_<tickFrequency_>) { }
    void disableSysTickIRQ() { }
};