#pragma once
#include <utility>
#include "types.hpp"
#include "peripheral_types.hpp"
#include "board/regmap/adc.hpp"
#include "board/pinout/adc.hpp"
#include "board/interrupts.hpp"
#include "adc.hpp"
#include "drivers/gpio/make.hpp"

#include "reg/set.hpp"
#include "reg/write.hpp"

namespace drivers::adc
{
    using Resolution = board::adc::CR1::ResVal;

    using Alignment = board::adc::CR2::AlignVal;

    template<std::uint8_t NPins>
    struct AdcPins
    {
        constexpr Pin operator[](std::uint8_t i) const
        {
            return pins[i];
        }

        Pin pins[NPins];
    };

    template<class T, class ... Ts>
    AdcPins(T, Ts...) -> AdcPins<sizeof...(Ts) + 1>;

    template<std::uint8_t NPins>
    struct AdcConfig
    {
        inline constexpr static std::uint8_t numberOfPins = NPins;

        std::uint8_t id;
        AdcPins<NPins> pins;
        Resolution resolution = Resolution::BITS12;
        Alignment alignment = Alignment::RIGHT;
    };

    template<std::uint8_t NPins>
    AdcConfig(std::uint8_t, AdcPins<NPins>, ...) -> AdcConfig<NPins>;

    namespace detail
    {
        template<class AdcX, std::uint8_t I, Pin pin>
        void configureChannel()
        {
            auto channelIndex = uint32_c<board::adc::AdcChannel<pin.port, pin.pin>::value>;
            reg::write(AdcX{}, 
                board::adc::SQR::SQ[uint8_c<I>], 
                channelIndex);
            reg::write(AdcX{}, 
                board::adc::SMPR::SMP[channelIndex],
                board::adc::SMPR::SmprVal::DIV56);
        }

        template<board::adc::CR1::ResVal resolution>
        constexpr std::uint16_t getMaxValue()
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

    template<std::uint8_t channelCount, AdcConfig<channelCount> config>
    struct makeAdc_t
    {
        template<class Board>
        constexpr auto operator()(Board boardDescriptor) const
        {
            constexpr auto adcX = boardDescriptor
                .getPeripheral(PeripheralTypes::ADC<config.id>);
            using AdcX = decltype(adcX);

            adcX.enable();

            // Configure pins and channels
            [boardDescriptor]<auto... Is>(std::index_sequence<Is...>)
            {
                (gpio::makeAnalogPin<gpio::AnalogPinConfig { .pin = config.pins[Is] }>(boardDescriptor), ...);
                (detail::configureChannel<AdcX, Is, config.pins[Is]>(), ...);
            }(std::make_index_sequence<channelCount>{});

            if constexpr (channelCount > 1)
            {
                reg::set(adcX, board::adc::CR1::SCAN);
            }

            reg::write(adcX, board::adc::SQR::L, uint32_c<channelCount>);
            reg::write(adcX, board::adc::CR1::RES, constant_c<config.resolution>);
            reg::apply(adcX,
                reg::write(board::adc::CR2::ALIGN, constant_c<config.alignment>),
                reg::set(board::adc::CR2::ADON));

            // Setup interrupts
            boardDescriptor.enableIRQ(board::Interrupts::ADC);
            auto interruptEvent = boardDescriptor.getInterruptEvent(board::Interrupts::ADC);

            return Adc<decltype(adcX), channelCount, detail::getMaxValue<config.resolution>()>{
                interruptEvent
            };
        }
    };

    template<auto config>
    inline constexpr makeAdc_t<decltype(config)::numberOfPins, config> makeAdc {};
}