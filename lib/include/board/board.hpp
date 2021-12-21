#pragma once
#include <ratio>
#include "peripherals.hpp"
#include "async/event.hpp"
#include "regmap/flash.hpp"
#include "regmap/nvic.hpp"
#include "regmap/stk.hpp"
#include "interrupts.hpp"
#include "peripheral_types.hpp"
#include "reg/apply.hpp"
#include "rational.hpp"

namespace board
{
	using Flash = Peripheral<flash::tag,
		DeviceMemory<std::uint32_t, 0x40023C00, 0x40023FFF>>;
    using Nvic = Peripheral<nvic::tag, 
        DeviceMemory<std::uint32_t, 0xE000E100, 0xE000E4EF>>;
    using Stk = Peripheral<stk::tag,
        DeviceMemory<std::uint32_t, 0xE000E010, 0xE000E020>>;

    template<class ClockConfig>
    class Board
    {
    public:
        using InterruptController = InterruptControl;

        constexpr Rational<std::uint32_t> getSystemClockFrequency() const
        {
            return ClockConfig::getSystemClockFrequency();
        }

        constexpr Rational<std::uint32_t> getAhbClockFrequency() const
        {
            return ClockConfig::getAhbClockFrequency();
        }

        constexpr Rational<std::uint32_t> getApb1ClockFrequency() const
        {
            return ClockConfig::getApb1ClockFrequency();
        }

        constexpr Rational<std::uint32_t> getApb2ClockFrequency() const
        {
            return ClockConfig::getApb2ClockFrequency();
        }

        constexpr Rational<std::uint32_t> getI2SClockFrequency() const
        {
            return ClockConfig::getI2SClockFrequency();
        }

        template<int number>
        void enableIRQ(Interrupt<number>)
        {
            reg::set(Nvic{}, nvic::ISER::SETENA[uint32_c<number>]);
        }

        template<int number>
        void disableIRQ(Interrupt<number>)
        {
            reg::set(Nvic{}, nvic::ICER::CLRENA[uint32_c<number>]);
        }

        InterruptController getInterruptController() const
        {
            return {};
        }

        template<std::uint32_t tickFrequency>
        void enableSysTickIRQ(uint32_<tickFrequency>)
        {
            constexpr auto ticksBetweenInterrupts = (ClockConfig::getSystemClockFrequency() / tickFrequency) - 1U;
            static_assert(ticksBetweenInterrupts > 1U, "Tick frequency is too low");
            static_assert(ticksBetweenInterrupts < 0xFFFFFF, "Tick frequency is too high");

            reg::write(Stk{}, stk::LOAD::RELOAD, uint32_c<round(ticksBetweenInterrupts)>);
            reg::write(Stk{}, stk::VAL::CURRENT, uint32_c<0>);
            reg::apply(Stk{},
                reg::set(stk::CTRL::ENABLE),
                reg::write(stk::CTRL::CLKSOURCE, constant_c<stk::CTRL::ClkSourceVal::PROCESSOR>),
                reg::set(stk::CTRL::TICKINT));
        }

        void disableSysTickIRQ()
        {
            reg::clear(Stk{}, stk::CTRL::ENABLE);
        }

        template<class T>
        constexpr T getPeripheral(T) const
        {
            static_assert(sizeof(T) == -1, "Could not find the requested peripheral");
            return {};
        }

        constexpr GpioA getPeripheral(PeripheralTypes::tags::Gpio<0>) const { return {}; }
        constexpr GpioB getPeripheral(PeripheralTypes::tags::Gpio<1>) const { return {}; }
        constexpr GpioC getPeripheral(PeripheralTypes::tags::Gpio<2>) const { return {}; }
        constexpr GpioD getPeripheral(PeripheralTypes::tags::Gpio<3>) const { return {}; }
        constexpr GpioE getPeripheral(PeripheralTypes::tags::Gpio<4>) const { return {}; }
        constexpr GpioF getPeripheral(PeripheralTypes::tags::Gpio<5>) const { return {}; }
        constexpr GpioG getPeripheral(PeripheralTypes::tags::Gpio<6>) const { return {}; }
        constexpr GpioH getPeripheral(PeripheralTypes::tags::Gpio<7>) const { return {}; }
        constexpr GpioI getPeripheral(PeripheralTypes::tags::Gpio<8>) const { return {}; }

        constexpr Exti getPeripheral(PeripheralTypes::tags::Exti) const { return {}; }
        constexpr SysCfg getPeripheral(PeripheralTypes::tags::SysCfg) const { return {}; }

        constexpr Uart4 getPeripheral(PeripheralTypes::tags::Uart<3>) const { return {}; }
        constexpr Uart5 getPeripheral(PeripheralTypes::tags::Uart<4>) const { return {}; }

        constexpr I2C1 getPeripheral(PeripheralTypes::tags::I2c<0>) const { return {}; }
        constexpr I2C2 getPeripheral(PeripheralTypes::tags::I2c<1>) const { return {}; }
        constexpr I2C3 getPeripheral(PeripheralTypes::tags::I2c<2>) const { return {}; }

        constexpr Spi2 getPeripheral(PeripheralTypes::tags::Spi<1>) const { return {}; }
        constexpr Spi3 getPeripheral(PeripheralTypes::tags::Spi<2>) const { return {}; }

        constexpr Dma1 getPeripheral(PeripheralTypes::tags::Dma<0>) const { return {}; }
        constexpr Dma2 getPeripheral(PeripheralTypes::tags::Dma<1>) const { return {}; }

        constexpr Adc1 getPeripheral(PeripheralTypes::tags::Adc<0>) const { return {}; }
        constexpr Adc2 getPeripheral(PeripheralTypes::tags::Adc<1>) const { return {}; }
        constexpr Adc3 getPeripheral(PeripheralTypes::tags::Adc<2>) const { return {}; }

        template<int irqNo>
        async::EventEmitter getInterruptEvent(Interrupt<irqNo>)
        {
            return detail::getInterruptEvent(irqNo);
        }
    };

    template<class ClockConfig>
    constexpr auto makeBoard(ClockConfig)
        -> Board<ClockConfig>
    {
        reg::apply(Flash{},
            reg::clear(flash::ACR::DCEN), // Disable data cache
            reg::clear(flash::ACR::ICEN)); // Disable instruction cache

        // Reset cache
        reg::apply(Flash{},
            reg::set(flash::ACR::DCRST),
            reg::set(flash::ACR::ICRST));

        reg::apply(Flash{}, 
            reg::write(flash::ACR::LATENCY, uint32_c<5>),
            reg::set(flash::ACR::DCEN), // Enable data cache
            reg::set(flash::ACR::ICEN), // Enable instruction cache
            reg::set(flash::ACR::PRFTEN)); // Enable pre-fetch

        ClockConfig::apply(Rcc{});
        return Board<ClockConfig>{};
    }
}
