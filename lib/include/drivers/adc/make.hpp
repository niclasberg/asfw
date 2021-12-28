#pragma once
#include "types.hpp"
#include "peripheral_types.hpp"
#include "board/regmap/adc.hpp"
#include "board/pinout/adc.hpp"
#include "board/interrupts.hpp"
#include "opt/option.hpp"
#include <utility>
#include "adc.hpp"
#include "drivers/gpio/make.hpp"

#include "reg/set.hpp"
#include "reg/write.hpp"

namespace drivers::adc
{
    namespace detail
    {
        MAKE_OPTION_KEY(RESOLUTION);
        MAKE_OPTION_KEY(ALIGN);
        MAKE_OPTION_KEY(CHANNELS);
        MAKE_OPTION_KEY(SAMPLING_TIME);

        template<class ... Pins> struct AdcPins;

        template<std::uint8_t ... portNos, std::uint8_t ... pinNos>
        struct AdcPins<Pin<portNos, pinNos>...> { };

        template<class AdcX, std::uint8_t I, std::uint8_t portNo, std::uint8_t pinNo>
        void configureChannel(AdcX adcX, uint8_<I>, Pin<portNo, pinNo> pin)
        {
            auto channelIndex = uint32_c<board::adc::AdcChannel<portNo, pinNo>::value>;
            reg::write(adcX, 
                board::adc::SQR::SQ[uint8_c<I>], 
                channelIndex);
            reg::write(adcX, 
                board::adc::SMPR::SMP[channelIndex],
                board::adc::SMPR::SmprVal::DIV56);
        }

        template<board::adc::CR1::ResVal resolution>
        constexpr std::uint16_t getMaxValue(constant_<resolution>)
        {
            if constexpr (resolution == board::adc::CR1::ResVal::BITS6)
            {
                return 1U << 6U;
            }
            else if constexpr (resolution == board::adc::CR1::ResVal::BITS8)
            {
                return 1U << 8U;
            }
            else if constexpr (resolution == board::adc::CR1::ResVal::BITS10)
            {
                return 1U << 10U;
            }
            else
            {
                return 1U << 12U;
            }
        }
    }

    namespace Resolution
    {
        constexpr auto BITS6  = opt::makeOption(detail::RESOLUTION, constant_c<board::adc::CR1::ResVal::BITS6>);
        constexpr auto BITS8  = opt::makeOption(detail::RESOLUTION, constant_c<board::adc::CR1::ResVal::BITS8>);
        constexpr auto BITS10 = opt::makeOption(detail::RESOLUTION, constant_c<board::adc::CR1::ResVal::BITS10>);
        constexpr auto BITS12 = opt::makeOption(detail::RESOLUTION, constant_c<board::adc::CR1::ResVal::BITS12>);
    }

    namespace Alignment
    {
        constexpr auto LEFT  = opt::makeOption(detail::ALIGN, constant_c<board::adc::CR2::AlignVal::LEFT>);
        constexpr auto RIGHT = opt::makeOption(detail::ALIGN, constant_c<board::adc::CR2::AlignVal::RIGHT>);
    }

    template<std::uint8_t ... portNos, std::uint8_t ... pinNos>
    constexpr auto analogPins(Pin<portNos, pinNos>...) 
        -> detail::AdcPins<Pin<portNos, pinNos>...>
    {
        return {};
    }

    template<class Board, std::uint8_t adcId, std::uint8_t ... portNos, std::uint8_t ... pinNos, class ... Options>
    constexpr auto makeAdc(
        Board boardDescriptor, 
        DeviceId<adcId>,
        detail::AdcPins<Pin<portNos, pinNos>...> pins, 
        Options && ... opts)
    {
        constexpr auto adcX = boardDescriptor.getPeripheral(PeripheralTypes::ADC<adcId>);
        constexpr std::uint8_t channelCount = sizeof...(portNos);

        auto options = opt::makeOptionSet(
			opt::fields(detail::RESOLUTION, detail::ALIGN),
			std::forward<Options>(opts)...);

        constexpr auto resolution = opt::getOrDefault(options, detail::RESOLUTION, Resolution::BITS12);
        constexpr auto alignment = opt::getOrDefault(options, detail::ALIGN, Alignment::RIGHT);

        adcX.enable();

        // Configure pins and channels
        [boardDescriptor]<typename ... Pins>(detail::AdcPins<Pins...>)
        {
            (gpio::makeAnalogPin(boardDescriptor, Pins{}), ...);
        }(pins);

        [adcX]<std::size_t... Is, typename... Pins>(std::index_sequence<Is...>, detail::AdcPins<Pins...>)
        {
            (detail::configureChannel(adcX, uint8_c<Is>, Pins{}), ...);
        }(std::make_index_sequence<channelCount>(), pins);

        if constexpr (channelCount > 1)
        {
            reg::set(adcX, board::adc::CR1::SCAN);
        }

        reg::write(adcX, board::adc::SQR::L, uint32_c<channelCount>);
        reg::write(adcX, board::adc::CR1::RES, resolution);
        reg::apply(adcX,
            reg::write(board::adc::CR2::ALIGN, alignment),
            reg::set(board::adc::CR2::ADON));

        // Setup interrupts
		boardDescriptor.enableIRQ(board::Interrupts::ADC);
        auto interruptEvent = boardDescriptor.getInterruptEvent(board::Interrupts::ADC);

        return Adc<decltype(adcX), channelCount, detail::getMaxValue(resolution)>{
            interruptEvent
        };
    }
}