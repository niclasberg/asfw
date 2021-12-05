#pragma once
#include "config.hpp"
#include "input_pin.hpp"
#include "output_pin.hpp"
#include "input_interrupt_pin.hpp"
#include "peripheral_types.hpp"
#include <boost/hana/string.hpp>
#include "opt/option.hpp"

namespace drivers::gpio
{
	namespace detail
	{
        MAKE_OPTION_KEY(OUTPUT_TYPE);
        MAKE_OPTION_KEY(PUPD);
        MAKE_OPTION_KEY(ALT_FUNCTION);
        MAKE_OPTION_KEY(SPEED);
        MAKE_OPTION_KEY(INTERRUPT);

		template<std::uint8_t pin>
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
    
	namespace AltFn
	{
		constexpr auto SYSTEM             = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::SYSTEM>);
		constexpr auto TIMER1_2           = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::TIMER1_2>);
		constexpr auto TIMER3_5           = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::TIMER3_5>);
		constexpr auto TIMER8_11          = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::TIMER8_11>);
		constexpr auto I2C                = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::I2C>);
		constexpr auto SPI1_2             = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::SPI1_2>);
		constexpr auto SPI3               = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::SPI3>);
		constexpr auto USART1_3           = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::USART1_3>);
		constexpr auto USART4_6           = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::USART4_6>);
		constexpr auto CAN1_2_TIMER_12_14 = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::CAN1_2_TIMER_12_14>);
		constexpr auto OTG_FS_HS          = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::OTG_FS_HS>);
		constexpr auto ETH                = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::ETH>);
		constexpr auto FSMC_SDIO_OTG_HS   = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::FSMC_SDIO_OTG_HS>);
		constexpr auto DCMI               = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::DCMI>);
		constexpr auto EVENTOUT           = opt::makeOption(detail::ALT_FUNCTION, constant_c<detail::AfrVal::EVENTOUT>);
	}

    namespace OutType
	{
        constexpr auto PUSH_PULL  = opt::makeOption(detail::OUTPUT_TYPE, constant_c<detail::OtyperVal::PUSH_PULL>);
		constexpr auto OPEN_DRAIN = opt::makeOption(detail::OUTPUT_TYPE, constant_c<detail::OtyperVal::OPEN_DRAIN>);
	}

	namespace PuPd
	{
		constexpr auto NO_PULL   = opt::makeOption(detail::PUPD, constant_c<detail::PupdrVal::NO_PULL>);
		constexpr auto PULL_UP   = opt::makeOption(detail::PUPD, constant_c<detail::PupdrVal::PULL_UP>);;
		constexpr auto PULL_DOWN = opt::makeOption(detail::PUPD, constant_c<detail::PupdrVal::PULL_DOWN>);;
	}
	
	namespace Speed
	{
		constexpr auto LOW       = opt::makeOption(detail::SPEED, constant_c<detail::OspeedrVal::LOW>);
		constexpr auto MEDIUM    = opt::makeOption(detail::SPEED, constant_c<detail::OspeedrVal::MEDIUM>);
		constexpr auto HIGH      = opt::makeOption(detail::SPEED, constant_c<detail::OspeedrVal::HIGH>);
		constexpr auto VERY_HIGH = opt::makeOption(detail::SPEED, constant_c<detail::OspeedrVal::VERY_HIGH>);
	}

	namespace Interrupt
	{
		constexpr auto RISING_EDGE = opt::makeOption(detail::INTERRUPT, hana::type_c<detail::RisingEdgeInterrupt>);
		constexpr auto FALLING_EDGE = opt::makeOption(detail::INTERRUPT, hana::type_c<detail::FallingEdgeInterrupt>);
		constexpr auto RISING_FALLING_EDGE = opt::makeOption(detail::INTERRUPT, hana::type_c<detail::RisingFallingEdgeInterrupt>);
	}

    /**
     * @brief Create an output pin gpio
     **/
	template<class Board, std::uint8_t portNo, std::uint8_t pinNo, class ... Options>
	constexpr auto makeOutputPin(
		Board board, 
		Pin<portNo, pinNo>,
		Options && ... opts)
	{
		constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<portNo>); 

        auto options = opt::makeOptionSet(
			opt::fields(detail::OUTPUT_TYPE, detail::SPEED, detail::PUPD),
			std::forward<Options>(opts)...);
		
        auto speed   = opt::getOrDefault(options, detail::SPEED, Speed::LOW);
        auto outType = opt::getOrDefault(options, detail::OUTPUT_TYPE, OutType::PUSH_PULL);
        auto pupd    = opt::getOrDefault(options, detail::PUPD, PuPd::NO_PULL);

        detail::OutputPinConfig<portNo, pinNo, 
			hana::value(speed), 
			hana::value(pupd), 
			hana::value(outType)>::apply(gpioX);

		return OutputPin<decltype(gpioX), portNo, pinNo>{};
	}

	template<class Board, 
		std::uint8_t portNo, 
		std::uint8_t pinNo, 
		class ... Options>
	auto makeInputPin(Board board, Pin<portNo, pinNo>, Options && ... opts)
	{
		constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<portNo>); 

        auto options = opt::makeOptionSet(opt::fields(detail::PUPD), std::forward<Options>(opts)...);
        auto pupd = opt::getOrDefault(options, detail::PUPD, PuPd::NO_PULL);

        detail::InputPinConfig<portNo, pinNo, hana::value(pupd)>::apply(gpioX);

		return InputPin<decltype(gpioX), portNo, pinNo>{};
	}

	template<class Board,std::uint8_t portNo, std::uint8_t pinNo>
	void makeAnalogPin(Board board, Pin<portNo, pinNo>)
	{
        detail::AnalogPinConfig<portNo, pinNo>::apply(board.getPeripheral(PeripheralTypes::GPIO<portNo>));
    }

	template<class Board, std::uint8_t portNo, std::uint8_t pinNo, class ... Options>
	void makeAltFnPin(Board board, Pin<portNo, pinNo>, Options && ... opts)
	{
		constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<portNo>); 

		auto options = opt::makeOptionSet(
			opt::fields(detail::OUTPUT_TYPE, detail::SPEED, detail::PUPD, detail::ALT_FUNCTION),
			std::forward<Options>(opts)...);

        auto speed   = opt::getOrDefault(options, detail::SPEED, Speed::LOW);
        auto outType = opt::getOrDefault(options, detail::OUTPUT_TYPE, OutType::PUSH_PULL);
        auto pupd    = opt::getOrDefault(options, detail::PUPD, PuPd::NO_PULL);
		auto altFn   = opt::get(options, detail::ALT_FUNCTION);

        detail::AltFnPinConfig<portNo, pinNo, 
            hana::value(altFn), 
            hana::value(speed), 
            hana::value(pupd), 
            hana::value(outType)>::apply(gpioX);
	}

	template<class Board, std::uint8_t portNo, std::uint8_t pinNo, class ... Options>
	auto makeInputInterruptPin(Board board, Pin<portNo, pinNo>, Options && ... opts)
	{
		constexpr auto gpioX = board.getPeripheral(PeripheralTypes::GPIO<portNo>); 
		constexpr auto exti = board.getPeripheral(PeripheralTypes::EXTI);
		constexpr auto syscfg = board.getPeripheral(PeripheralTypes::SYSCFG);

		auto options = opt::makeOptionSet(
			opt::fields(detail::PUPD, detail::INTERRUPT),
			std::forward<Options>(opts)...);

		static_assert(hana::contains(options, detail::INTERRUPT), "Interrupt type must be supplied");

        auto pupd = opt::getOrDefault(options, detail::PUPD, PuPd::NO_PULL);
		auto interruptType = opt::get(options, detail::INTERRUPT);
		using InterruptConfig = typename decltype(interruptType)::type;

		detail::InputPinConfig<portNo, pinNo, hana::value(pupd)>::apply(gpioX);
		detail::InterruptBaseConfig::apply(exti, syscfg, uint8_c<portNo>, uint8_c<pinNo>);
		InterruptConfig::apply(exti, syscfg, uint8_c<portNo>, uint8_c<pinNo>);

		constexpr auto interrupt = detail::getInterrupt(uint8_c<pinNo>);
		board.enableIRQ(interrupt);

		return InputInterruptPin<
			decltype(gpioX), 
			decltype(exti),  
			portNo, 
			pinNo>(board.getInterruptEvent(interrupt));
	}

}
