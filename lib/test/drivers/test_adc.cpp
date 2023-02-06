#include "../catch.hpp"
#include "drivers/adc.hpp"
#include "../mocks/mock_board.hpp"
#include "../mocks/mock_peripheral.hpp"

using MockAdc = MockPeripheral<board::adc::tag>;
using MockGpio = MockPeripheral<board::gpio::tag>;

namespace
{
    async::Event adcInterruptEvent;

    struct MockPeripherals
    {
        constexpr MockGpio getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }
        constexpr MockAdc getPeripheral(PeripheralTypes::tags::Adc<0>) const { return {}; }

        async::EventEmitter getInterruptEvent(decltype(board::Interrupts::ADC)) { return {&adcInterruptEvent}; }
    };
}

TEST_CASE("ADC")
{
    MockBoard<MockPeripherals> mockBoard;

    using namespace drivers::adc;
    
    auto dev = makeAdc<AdcConfig<2> {
        .id = 0,
        .pins = { Pin(0, 0), Pin(0, 1) }
    }>(mockBoard);
}