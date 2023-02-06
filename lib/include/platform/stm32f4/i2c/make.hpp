#pragma once
#include "types.hpp"
#include "peripheral_types.hpp"
#include "i2c_master.hpp"
#include "board/regmap/i2c.hpp"
#include "board/interrupts.hpp"
#include "drivers/gpio.hpp"

#include "reg/write.hpp"
#include "reg/set.hpp"

namespace platform::stm32f4::i2c
{
	struct MasterConfig
	{
		std::uint8_t deviceId;
		Pin clockPin;
		Pin dataPin;
		std::uint32_t serialClockFrequency = 100'000;
	};

	namespace detail
	{
		// Mapping between i2c id and interrupt
		template<std::uint8_t deviceId>
        constexpr auto getI2cErrorInterrupt(DeviceId<deviceId>) 
		{ 
			static_assert(deviceId < 3, "Device id out of range");
            if constexpr (deviceId == 0)
			{
				return board::Interrupts::I2C1_ER;
			}
			else if constexpr (deviceId == 1)
			{
				return board::Interrupts::I2C2_ER;
			}
			else
			{
				return board::Interrupts::I2C3_ER;
			}
		}

		template<std::uint8_t deviceId>
        constexpr auto getI2cEventInterrupt(DeviceId<deviceId>) 
		{ 
			static_assert(deviceId < 3, "Device id out of range");
            if constexpr (deviceId == 0)
			{
				return board::Interrupts::I2C1_EV;
			}
			else if constexpr (deviceId == 1)
			{
				return board::Interrupts::I2C2_EV;
			}
			else
			{
				return board::Interrupts::I2C3_EV;
			}
		}
	}

	template<MasterConfig config>
	struct makeI2c_t
	{
		template<class Board>
		constexpr auto operator()(Board boardDescriptor) const
			-> I2cMaster<decltype(std::declval<Board>().getPeripheral(PeripheralTypes::I2C<config.deviceId>))>
		{
			auto i2cX = boardDescriptor.getPeripheral(PeripheralTypes::I2C<config.deviceId>);

			// Initialize pins
			gpio::makeAltFnPin<gpio::AltFnPinConfig {
				.pin = config.clockPin,
				.altFn = gpio::AltFn::I2C,
				.outType = gpio::OutType::OPEN_DRAIN, 
				.pullUpDown = gpio::PuPd::NO_PULL
			}>(boardDescriptor);

			gpio::makeAltFnPin<gpio::AltFnPinConfig {
				.pin = config.dataPin,
				.altFn = gpio::AltFn::I2C,
				.outType = gpio::OutType::OPEN_DRAIN, 
				.pullUpDown = gpio::PuPd::NO_PULL
			}>(boardDescriptor);

			// Initialize clocks
			constexpr auto pClkFreq = boardDescriptor.getApb1ClockFrequency();
			static_assert(pClkFreq <= 50'000'000U, 
				"I2C requires a peripheral clock frequency less than or equal to 50 MHz");
			static_assert(pClkFreq >= 2'000'000U, 
				"I2C requires a peripheral clock frequency greater than or equal to 2 MHz");

			i2cX.enable();
			reg::write(i2cX, board::i2c::CR2::FREQ, uint32_c<round(pClkFreq / 1'000'000U)>);

			if constexpr (config.serialClockFrequency <= 100'000)
			{
				// Initialize in standard mode
				constexpr auto ccr = uint32_c<round(pClkFreq) / (2 * config.serialClockFrequency)>;
				constexpr auto trise = uint32_c<round(pClkFreq) / 1'000'000 + 1>;

				// Enable standard mode i2c
				reg::apply(i2cX, 
					reg::write(board::i2c::CCR::F_S, uint32_c<0>),
					reg::write(board::i2c::CCR::CCR, ccr));
				reg::write(i2cX, board::i2c::TRISE::TRISE, trise);
			}
			else
			{
				static_assert(config.serialClockFrequency == -1, "Fast mode is not supported by this driver");
			}

			// Enable peripheral
			reg::set(i2cX, board::i2c::CR1::PE);

			// Enable interrupts
			auto evtInterrupt = detail::getI2cEventInterrupt(DeviceId<config.deviceId>{});
			auto errInterrupt = detail::getI2cErrorInterrupt(DeviceId<config.deviceId>{});
			boardDescriptor.enableIRQ(evtInterrupt);
			boardDescriptor.enableIRQ(errInterrupt);

			return I2cMaster<decltype(i2cX)>{
				boardDescriptor.getInterruptEvent(evtInterrupt), 
				boardDescriptor.getInterruptEvent(errInterrupt)
			};
		}
	};

	template<MasterConfig config>
	inline constexpr makeI2c_t<config> makeI2c{};
}