#pragma once
#include "input_pin.hpp"
#include "output_pin.hpp"
#include "input_interrupt_pin.hpp"
#include "peripheral_types.hpp"

#include "board/regmap/gpio.hpp"
#include "board/regmap/exti.hpp"
#include "board/regmap/syscfg.hpp"
#include "board/interrupts.hpp"

namespace drivers::gpio
{
	// Alternate function
	using AltFn = board::gpio::AFR::AfrVal;

	// Pull up resistor configuration
	using PuPd = board::gpio::PUPDR::PupdrVal;

	// GPIO speed
	using Speed = board::gpio::OSPEEDR::OspeedrVal;

	// Output type
	using OutType = board::gpio::OTYPER::OtyperVal;

	// Interrupt configuration
	enum class Interrupt
	{
		RISING_EDGE,
		FALLING_EDGE,
		RISING_FALLING_EDGE
	};

	struct InputPinConfig
	{
		Pin pin;
		PuPd pullUpDown = PuPd::NO_PULL;
	};

	struct OutputPinConfig
	{
		Pin pin;
		OutType outType = OutType::PUSH_PULL;
		PuPd pullUpDown = PuPd::NO_PULL;
		Speed speed = Speed::LOW;
	};

	struct AnalogPinConfig
	{
		Pin pin;
	};

	struct AltFnPinConfig
	{
		Pin pin;
		AltFn altFn;
		OutType outType = OutType::PUSH_PULL;
		PuPd pullUpDown = PuPd::NO_PULL;
		Speed speed = Speed::LOW;
	};

	struct InputInterruptPinConfig
	{
		Pin pin;
		Interrupt interrupt;
		PuPd pullUpDown = PuPd::NO_PULL;
	};

	using board::gpio::MODER::ModerVal;

	/**
	 * @brief Create an output gpio pin
	 **/
	template <OutputPinConfig config>
	struct makeOutputPin_t
	{
		template <class Board>
		constexpr auto operator()(Board board) const
		{
			using board::gpio::MODER::ModerVal;
			constexpr auto pinIndex = uint8_c<config.pin.pin>;
			constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<config.pin.port>);

			gpioX.enable();
			reg::write(gpioX, board::gpio::MODER::MODER[pinIndex], constant_c<ModerVal::OUTPUT>);
			reg::write(gpioX, board::gpio::OSPEEDR::OSPEEDR[pinIndex], constant_c<config.speed>);
			reg::write(gpioX, board::gpio::PUPDR::PUPDR[pinIndex], constant_c<config.pullUpDown>);
			reg::write(gpioX, board::gpio::OTYPER::OT[pinIndex], constant_c<config.outType>);

			return OutputPin<decltype(gpioX), config.pin.port, config.pin.pin>{};
		}
	};

	template <OutputPinConfig config>
	inline constexpr makeOutputPin_t<config> makeOutputPin{};

	template <InputPinConfig config>
	struct makeInputPin_t
	{
		template <class Board>
		constexpr auto operator()(Board board) const
		{
			using board::gpio::MODER::ModerVal;
			constexpr auto pinIndex = uint8_c<config.pin.pin>;
			constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<config.pin.port>);

			gpioX.enable();
			reg::write(gpioX, board::gpio::MODER::MODER[pinIndex], constant_c<ModerVal::INPUT>);
			reg::write(gpioX, board::gpio::PUPDR::PUPDR[pinIndex], constant_c<config.pullUpDown>);

			return InputPin<decltype(gpioX), config.pin.port, config.pin.pin>{};
		}
	};

	template <InputPinConfig config>
	inline constexpr makeInputPin_t<config> makeInputPin{};

	template <AnalogPinConfig config>
	struct makeAnalogPin_t
	{
		template <class Board>
		constexpr void operator()(Board board) const
		{
			using board::gpio::MODER::ModerVal;
			constexpr auto pinIndex = uint8_c<config.pin.pin>;
			constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<config.pin.port>);

			gpioX.enable();
			reg::write(gpioX, board::gpio::MODER::MODER[pinIndex], constant_c<ModerVal::ANALOG>);
		}
	};

	template <AnalogPinConfig config>
	inline constexpr makeAnalogPin_t<config> makeAnalogPin{};

	template <AltFnPinConfig config>
	struct makeAltFnPin_t
	{
		template <class Board>
		constexpr void operator()(Board board) const
		{
			using board::gpio::MODER::ModerVal;
			constexpr auto pinIndex = uint8_c<config.pin.pin>;
			constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<config.pin.port>);

			gpioX.enable();
			reg::write(gpioX, board::gpio::MODER::MODER[pinIndex], constant_c<ModerVal::ALTERNATE_FUNCTION>);
			reg::write(gpioX, board::gpio::OSPEEDR::OSPEEDR[pinIndex], constant_c<config.speed>);
			reg::write(gpioX, board::gpio::PUPDR::PUPDR[pinIndex], constant_c<config.pullUpDown>);
			reg::write(gpioX, board::gpio::OTYPER::OT[pinIndex], constant_c<config.outType>);
			reg::write(gpioX, board::gpio::AFR::AFR[pinIndex], constant_c<config.altFn>);
		}
	};

	template <AltFnPinConfig config>
	inline constexpr makeAltFnPin_t<config> makeAltFnPin{};

	namespace detail
	{
		template <std::uint8_t pin>
		constexpr auto getInterrupt(uint8_<pin>)
		{
			static_assert(pin < 16, "Invalid pin number (should be < 16)");
			if constexpr (pin == 0)
				return board::Interrupts::EXTI0;
			else if constexpr (pin == 1)
				return board::Interrupts::EXTI1;
			else if constexpr (pin == 2)
				return board::Interrupts::EXTI2;
			else if constexpr (pin == 3)
				return board::Interrupts::EXTI3;
			else if constexpr (pin == 4)
				return board::Interrupts::EXTI4;
			else if constexpr (pin <= 9)
				return board::Interrupts::EXTI9_5;
			else if constexpr (pin <= 16)
				return board::Interrupts::EXTI15_10;
		}
	}

	template <InputInterruptPinConfig config>
	struct makeInputInterruptPin_t
	{
		template <class Board>
		constexpr auto operator()(Board board) const
		{
			using board::gpio::MODER::ModerVal;
			constexpr auto pinIndex = uint8_c<config.pin.pin>;
			constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<config.pin.port>);
			constexpr auto exti = board.getPeripheral(PeripheralTypes::EXTI);
			constexpr auto syscfg = board.getPeripheral(PeripheralTypes::SYSCFG);

			gpioX.enable();
			syscfg.enable();
			exti.enable();

			reg::write(gpioX, board::gpio::MODER::MODER[pinIndex], constant_c<ModerVal::INPUT>);
			reg::write(gpioX, board::gpio::PUPDR::PUPDR[pinIndex], constant_c<config.pullUpDown>);
			reg::write(syscfg, board::syscfg::EXTICR::EXTI[pinIndex], uint32_c<config.pin.port>);

			if constexpr (config.interrupt == Interrupt::RISING_EDGE)
			{
				reg::set(exti, board::exti::RTSR::TR[pinIndex]);
				reg::clear(exti, board::exti::FTSR::TR[pinIndex]);
			}
			else if constexpr (config.interrupt == Interrupt::FALLING_EDGE)
			{
				reg::clear(exti, board::exti::RTSR::TR[pinIndex]);
				reg::set(exti, board::exti::FTSR::TR[pinIndex]);
			}
			else
			{
				reg::set(exti, board::exti::RTSR::TR[pinIndex]);
				reg::set(exti, board::exti::FTSR::TR[pinIndex]);
			}

			constexpr auto interrupt = detail::getInterrupt(pinIndex);
			board.enableIRQ(interrupt);

			return InputInterruptPin<
				decltype(gpioX),
				decltype(exti),
				config.pin.port,
				config.pin.pin>(board.getInterruptEvent(interrupt));
		}
	};

	template <InputInterruptPinConfig config>
	inline constexpr makeInputInterruptPin_t<config> makeInputInterruptPin{};
}
