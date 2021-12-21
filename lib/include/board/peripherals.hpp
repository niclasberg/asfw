#pragma once
#include "peripheral.hpp"
#include "regmap/gpio.hpp"
#include "regmap/adc.hpp"
#include "regmap/spi.hpp"
#include "regmap/exti.hpp"
#include "regmap/syscfg.hpp"
#include "regmap/rcc.hpp"
#include "regmap/i2c.hpp"
#include "regmap/dma.hpp"
#include "regmap/uart.hpp"
#include "reg/peripheral_operations.hpp"

namespace board
{
	using Rcc = Peripheral<rcc::tag, 
        DeviceMemory<std::uint32_t, 0x40023800, 0x40023BFF>>;

	namespace detail
	{
        // Peripheral clock enable methods
		template<class RccEnableRegister, class RccResetRegister>
		struct PeriphCtrl
		{
			static INLINE void enable()
			{
                reg::set(Rcc{}, RccEnableRegister{});
			}

			static INLINE void disable()
			{
                reg:clear(Rcc{}, RccEnableRegister{});
			}

			static INLINE void reset()
			{
				reg::set(Rcc{}, RccResetRegister{});
                reg::clear(Rcc{}, RccResetRegister{});
			}
		};
	}

	// Peripherals hanging on AHB1 bus
    using GpioA = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40020000, 0x400203FF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOAEN),
				decltype(rcc::AHB1RSTR::GPIOARST)>>;
    using GpioB = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40020400, 0x400207FF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOBEN),
				decltype(rcc::AHB1RSTR::GPIOBRST)>>;
    using GpioC = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40020800, 0x40020BFF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOCEN),
				decltype(rcc::AHB1RSTR::GPIOCRST)>>;
    using GpioD = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40020C00, 0x40020FFF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIODEN),
				decltype(rcc::AHB1RSTR::GPIODRST)>>;
    using GpioE = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40021000, 0x400213FF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOEEN),
				decltype(rcc::AHB1RSTR::GPIOERST)>>;
    using GpioF = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40021400, 0x400217FF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOFEN),
				decltype(rcc::AHB1RSTR::GPIOFRST)>>;
    using GpioG = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40021800, 0x40021BFF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOGEN),
				decltype(rcc::AHB1RSTR::GPIOGRST)>>;
    using GpioH = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40021C00, 0x40021FFF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOHEN),
				decltype(rcc::AHB1RSTR::GPIOHRST)>>;
    using GpioI = Peripheral<
            gpio::tag,
            DeviceMemory<std::uint32_t, 0x40022000, 0x400223FF>,
            detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::GPIOIEN),
				decltype(rcc::AHB1RSTR::GPIOIRST)>>;

	using Dma1 = Peripheral<
			dma::tag,
			DeviceMemory<std::uint32_t, 0x40026000, 0x400263FF>,
			detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::DMA1EN),
				decltype(rcc::AHB1RSTR::DMA1RST)>>;
	using Dma2 = Peripheral<
			dma::tag,
			DeviceMemory<std::uint32_t, 0x40026400, 0x400267FF>,
			detail::PeriphCtrl<
				decltype(rcc::AHB1ENR::DMA2EN),
				decltype(rcc::AHB1RSTR::DMA2RST)>>;

	// Peripherals hanging on the APB1 bus
	using Spi2 = Peripheral<
            spi::tag,
			DeviceMemory<uint32_t, 0x40003800, 0x40003BFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::SPI2EN),
				decltype(rcc::APB1RSTR::SPI2RST)>>;
	using Spi3 = Peripheral<
            spi::tag,
			DeviceMemory<uint32_t, 0x40003C00, 0x40003FFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::SPI3EN),
				decltype(rcc::APB1RSTR::SPI3RST)>>;

	using Uart4 = Peripheral<uart::tag, 
			DeviceMemory<uint32_t, 0x40004C00, 0x40004FFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::UART4EN),
				decltype(rcc::APB1RSTR::UART4RST)>>;
	using Uart5 = Peripheral<uart::tag, 
			DeviceMemory<uint32_t, 0x40005000, 0x400053FF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::UART5EN),
				decltype(rcc::APB1RSTR::UART5RST)>>;
	
	struct I2C1 : Peripheral<i2c::tag,
			DeviceMemory<uint32_t, 0x40005400, 0x400057FF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::I2C1EN),
				decltype(rcc::APB1RSTR::I2C1RST)>> {};
	struct I2C2 : Peripheral<i2c::tag,
			DeviceMemory<uint32_t, 0x40005800, 0x40005BFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::I2C2EN),
				decltype(rcc::APB1RSTR::I2C2RST)>> {};
	struct I2C3 : Peripheral<i2c::tag,
			DeviceMemory<uint32_t, 0x40005C00, 0x40005FFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB1ENR::I2C3EN),
				decltype(rcc::APB1RSTR::I2C3RST)>> {};

    // Peripherals hanging on the APB2 bus
	using Adc1 = Peripheral<
			adc::tag,
			DeviceMemory<std::uint32_t, 0x40012000, 0x4001204C>,
			detail::PeriphCtrl<
				decltype(rcc::APB2ENR::ADC1EN),
				decltype(rcc::APB2RSTR::ADCRST)>>;
	using Adc2 = Peripheral<
			adc::tag,
			DeviceMemory<std::uint32_t, 0x40012100, 0x4001214C>,
			detail::PeriphCtrl<
				decltype(rcc::APB2ENR::ADC2EN),
				decltype(rcc::APB2RSTR::ADCRST)>>;
	using Adc3 = Peripheral<
			adc::tag,
			DeviceMemory<std::uint32_t, 0x40012200, 0x4001224C>,
			detail::PeriphCtrl<
				decltype(rcc::APB2ENR::ADC3EN),
				decltype(rcc::APB2RSTR::ADCRST)>>;
	
    using SysCfg = Peripheral<
            syscfg::tag,
			DeviceMemory<std::uint32_t, 0x40013800, 0x40013BFF>,
			detail::PeriphCtrl<
				decltype(rcc::APB2ENR::SYSCFGEN),
				decltype(rcc::APB2RSTR::SYSCFGRST)>>;
    using Exti = Peripheral<
            exti::tag,
			DeviceMemory<uint32_t, 0x40013C00, 0x40013FFF>>;

	/*using Spi1 = Peripheral<
			spi::tag,
			DeviceMemory<std::uint32_t, SPI1_BASEADDR, 0x24>,
			detail::PeriphCtrl<
				decltype(rcc::APB2ENR::SPI1EN),
				decltype(rcc::APB2RSTR::SPI1RST)>>;*/
}
