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

template<
    class MockPeripherals,
    std::uint32_t ahbClockFrequency,
    std::uint32_t apb1ClockFrequency,
    std::uint32_t apb2ClockFrequency,
    std::uint32_t i2sClockFrequency>
class MockBoard : public MockPeripherals
{
    std::vector<bool> interruptIsEnabled;
public:
    using InterruptController = MockInterruptController;

    MockBoard() : interruptIsEnabled(128, false) { }

    constexpr Rational<std::uint32_t> getApb1ClockFrequency() const { return Rational{apb1ClockFrequency}; }
    constexpr Rational<std::uint32_t> getApb2ClockFrequency() const { return Rational{apb2ClockFrequency}; }
    constexpr Rational<std::uint32_t> getAhbClockFrequency() const { return Rational{ahbClockFrequency}; }
    constexpr Rational<std::uint32_t> getI2SClockFrequency() const { return Rational{i2sClockFrequency}; }

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

template<class T>
constexpr auto mockPeripherals = opt::makeOption(::detail::MOCK_PERIPHERALS, type_c<std::remove_cvref_t<T>>);

template<std::uint32_t f>
constexpr auto ahbClockFrequency = opt::makeOption(::detail::AHB_FREQUENCY, uint32_c<f>);

template<std::uint32_t f>
constexpr auto apb1ClockFrequency = opt::makeOption(::detail::APB1_FREQUENCY, uint32_c<f>);

template<std::uint32_t f>
constexpr auto apb2ClockFrequency = opt::makeOption(::detail::APB2_FREQUENCY, uint32_c<f>);

template<std::uint32_t f>
constexpr auto is2ClockFrequency = opt::makeOption(::detail::I2S_FREQUENCY, uint32_c<f>);

template<class ... Options>
auto makeMockBoard(Options ... opts)
{
    auto options = hana::make_map(opts...);

    auto mocks = hana::find(options, ::detail::MOCK_PERIPHERALS).value_or(type_c<::detail::DefaultMockPeripherals>);
    constexpr auto ahbFreq = hana::find(options, ::detail::AHB_FREQUENCY).value_or(uint32_c<0>);
    constexpr auto apb1Freq = hana::find(options, ::detail::APB1_FREQUENCY).value_or(uint32_c<0>);
    constexpr auto apb2Freq = hana::find(options, ::detail::APB2_FREQUENCY).value_or(uint32_c<0>);
    constexpr auto i2sFreq = hana::find(options, ::detail::I2S_FREQUENCY).value_or(uint32_c<0>);

    return MockBoard<
        typename decltype(mocks)::type,
        hana::value(ahbFreq), 
        hana::value(apb1Freq),
        hana::value(apb2Freq),
        hana::value(i2sFreq)>{};
}