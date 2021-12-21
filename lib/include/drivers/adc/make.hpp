#pragma once
#include "types.hpp"
#include "peripheral_types.hpp"
#include "board/regmap/adc.hpp"
#include "board/pinout/adc.hpp"
#include "opt/option.hpp"
//#include "adc.hpp"

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
        struct AdcPins<Pin<portNos, pinNos>...>
        {
            
        };

        template<std::uint8_t channelIndex, std::uint8_t portNo, std::uint8_t pinNo>
        void configurePin(uint8_<channelIndex>, Pin<portNo, pinNo> pin)
        {
            regWrite(adcX, 
                board::adc::SQR::SQ[uint8_c<channelIndex>], 
                uint32_c<board::adc::AdcChannel<portNo, pinNo>::value>);
            regWrite(adcX, 
                board::adc::SMPR::SMP[uint8_c<channelIndex>],
                board::adc::SMPR::SmprVal::DIV28);
        }

        template<class Board, std::uint8_t ... portNos, std::uint8_t ... pinNos>
        void setAsAnalogPins(Board boardDescriptor, AdcPins<Pin<portNos, pinNos>...>)
        {
            (gpio::makeAnalogPin(boardDescriptor, PIN<portNos, pinNos>), ...);
        }
    }

    namespace Resolution
    {
        constexpr auto BITS6  = opt::makeOption(detail::RESOLUTION, constant_c<detail::ResVal::BITS6>);
        constexpr auto BITS8  = opt::makeOption(detail::RESOLUTION, constant_c<detail::ResVal::BITS8>);
        constexpr auto BITS10 = opt::makeOption(detail::RESOLUTION, constant_c<detail::ResVal::BITS10>);
        constexpr auto BITS12 = opt::makeOption(detail::RESOLUTION, constant_c<detail::ResVal::BITS12>);
    }

    namespace Alignment
    {
        constexpr auto LEFT  = opt::makeOption(detail::ALIGN, constant_c<detail::AlignVal::LEFT>);
        constexpr auto RIGHT = opt::makeOption(detail::ALIGN, constant_c<detail::AlignVal::RIGHT>);
    }

    template<std::uint8_t ... portNos, std::uint8_t ... pinNos>
    constexpr auto analogPins(Pin<portNos, pinNo>...) 
        -> detail::AdcPins<Pin<portNos, pinNo>...>
    {
        return {};
    }

    template<class Board, std::uint8_t adcId, std::uint8_t ... portNos, std::uint8_t ... pinNos, class ... Options>
    constexpr auto makeAdc(
        Board boardDescriptor, 
        DeviceId<adcId>,
        detail::AdcPin<Pin<portNos, pinNos>...> pins, 
        Options && ... opts)
    {
        constexpr auto adcX = board.getPeripheral(PeripheralTypes::ADC<adcId>);

        auto options = hana::make_map(std::forward<Options>(opts)...);

        // 
        constexpr auto resolution = hana::find(options, detail::RESOLUTION).value_or(hana::second(Resolution::BITS12));
        constexpr auto alignment = hana::find(options, detail::ALIGN).value_or(hana::second(Alignment::RIGHT));

        if(channelCount > 1)
        {
            reg::set(adcX, board::adc::CR1::SCAN);
        }

        reg::write(adcX, board::adc::SQR::L, uint32_c<sizeof...(portNos)>);
        reg::write(adcX, board::adc::CR1::RES, resolution);
        reg::write(adcX, board::adc::CR2::ALIGN, alignment);

        // Configure pins and channels
        detail::setAsAnalogPins(boardDescriptor, pins);

        using Config = detail::AdcConfig<
            hana::value(channelCount),
            hana::value(resolution),
            pins.>;

        return Adc<AdcX, Config>{};
    }
}

}