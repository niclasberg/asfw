#pragma once
#include "types.hpp"
#include "peripheral_types.hpp"
#include "i2c_master.hpp"
#include "board/regmap/i2c.hpp"
#include "opt/option.hpp"
#include "detail/init.hpp"
#include "board/interrupts.hpp"
#include "drivers/gpio.hpp"

#include "reg/write.hpp"
#include "reg/set.hpp"

namespace drivers::i2c
{
	namespace detail
	{
		MAKE_OPTION_KEY(SCKL_FREQUENCY);
		MAKE_OPTION_KEY(SCKL_PIN);
		MAKE_OPTION_KEY(SDA_PIN);

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

	template<std::uint32_t freq> 
	constexpr auto serialClockFrequency = opt::makeOption(detail::SCKL_FREQUENCY, uint32_c<freq>);

	template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto clockPin = opt::makeOption(detail::SCKL_PIN, PIN<port, pin>);

	template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto dataPin = opt::makeOption(detail::SDA_PIN, PIN<port, pin>);

    template<class Board, std::uint8_t id, class ... Options>
    constexpr auto makeI2CMaster(Board boardDescriptor, DeviceId<id> deviceId, Options && ... opts)
		-> I2cMaster<decltype(std::declval<Board>().getPeripheral(PeripheralTypes::I2C<id>))>
    {
		auto i2cX = boardDescriptor.getPeripheral(PeripheralTypes::I2C<id>);

		auto options = opt::makeOptionSet(
            opt::fields(
                detail::SCKL_FREQUENCY,
				detail::SCKL_PIN,
				detail::SDA_PIN),
            std::forward<Options>(opts)...);

		// Initialize pins
		gpio::makeAltFnPin(
			boardDescriptor,
			opt::get(options, detail::SCKL_PIN), 
			gpio::AltFn::I2C, 
			gpio::OutType::OPEN_DRAIN, 
			gpio::PuPd::NO_PULL);

    	gpio::makeAltFnPin(
			boardDescriptor,
			opt::get(options, detail::SDA_PIN), 
			gpio::AltFn::I2C, 
			gpio::OutType::OPEN_DRAIN, 
			gpio::PuPd::NO_PULL);

		// Initialize clocks
		constexpr auto pClkFreq = boardDescriptor.getApb1ClockFrequency();
		static_assert(pClkFreq <= 50'000'000U, 
			"I2C requires a peripheral clock frequency less than or equal to 50 MHz");
		static_assert(pClkFreq >= 2'000'000U, 
			"I2C requires a peripheral clock frequency greater than or equal to 2 MHz");

		i2cX.enable();
		reg::write(i2cX, board::i2c::CR2::FREQ, 
			uint32_c<round(pClkFreq / 1'000'000U)>);
        detail::initClocks(
			i2cX, 
			constant_c<round(pClkFreq)>, 
			opt::getOrDefault(options, detail::SCKL_FREQUENCY, uint32_c<100'000>));
		reg::set(i2cX, board::i2c::CR1::PE);

		// Enable interrupts
		auto evtInterrupt = detail::getI2cEventInterrupt(deviceId);
		auto errInterrupt = detail::getI2cErrorInterrupt(deviceId);
		boardDescriptor.enableIRQ(evtInterrupt);
        boardDescriptor.enableIRQ(errInterrupt);

		return I2cMaster<decltype(i2cX)>{
			boardDescriptor.getInterruptEvent(evtInterrupt), 
			boardDescriptor.getInterruptEvent(errInterrupt)
		};
    }
}