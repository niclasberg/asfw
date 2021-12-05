#pragma once
#include "config.hpp"
#include <boost/hana/string.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/is_subset.hpp>

namespace board
{

namespace clock
{
    namespace detail
    {
        constexpr auto PLL_SOURCE = hana::string_c<'P', 'L', 'L', 'S', 'R', 'C'>;
        constexpr auto SYSCLK_SOURCE = hana::string_c<'S', 'Y', 'S', 'C', 'L', 'K'>;
        constexpr auto I2SCLK_SOURCE = hana::string_c<'S', 'Y', 'S', 'I', '2', 'S'>;
        constexpr auto AHB_PRE = hana::string_c<'A', 'H', 'B', 'P', 'R', 'E'>;
        constexpr auto APB1_PRE = hana::string_c<'A', 'P', 'B', '1', 'P', 'R', 'E'>;
        constexpr auto APB2_PRE = hana::string_c<'A', 'P', 'B', '2', 'P', 'R', 'E'>;

        template<std::uint32_t value> struct ClockDivide : uint32_<value> { };
        template<std::uint32_t value> struct ClockMultiply : uint32_<value> { };
        template<SwVal source> struct SysClkSrc : integral_constant<SwVal, source> {};
        template<SwVal source> constexpr SysClkSrc<source> sysClkSrc{};

        struct VoidConfigFactory
        {
            template<class Input>
            constexpr auto operator()(Input &&) const -> VoidConfig
            {
                return {};
            }
        };

        template<std::uint32_t mult, std::uint32_t div>
        struct SystemPllConfigFactory
        {
            template<class PllInput>
            constexpr auto operator()(PllInput &&) const -> SystemPllConfig<std::remove_cv_t<PllInput>, mult, div>
            {
                return {};
            }
        };

        template<std::uint32_t mult, std::uint32_t div>
        struct I2SPllConfigFactory
        {
            template<class PllInput>
            constexpr auto operator()(PllInput &&) const -> I2SPllConfig<std::remove_cv_t<PllInput>, mult, div>
            {
                return {};
            }
        };

        template<PllSrcVal pllSource, std::uint32_t div>
        struct PllInputFactory
        {
            template<class InputClock>
            constexpr auto operator()(InputClock &&) const -> PllInputConfig<std::remove_cv_t<InputClock>, pllSource, div>
            {
                return {};
            }
        };
    }

    template<std::uint32_t value> 
    constexpr detail::ClockDivide<value> divide{};
    
    template<std::uint32_t value> 
    constexpr detail::ClockMultiply<value> multiply{};

    namespace PllSource
    {
        constexpr auto HSI = integral_c<detail::PllSrcVal, detail::PllSrcVal::HSI>;
        constexpr auto HSE = integral_c<detail::PllSrcVal, detail::PllSrcVal::HSE>;
    }

    namespace SystemClockSource
    {
        constexpr auto HSI = detail::sysClkSrc<detail::SwVal::HSI>;
        constexpr auto HSE = detail::sysClkSrc<detail::SwVal::HSE>;
        constexpr auto PLL = detail::sysClkSrc<detail::SwVal::PLL>;
    }

    namespace AhbPrescaler
    {
        constexpr auto NO_DIV = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::NO_DIV>);
        constexpr auto DIV2   = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV2>);
        constexpr auto DIV4   = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV4>);
        constexpr auto DIV8   = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV8>);
        constexpr auto DIV16  = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV16>);
        constexpr auto DIV64  = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV64>);
        constexpr auto DIV128 = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV128>);
        constexpr auto DIV256 = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV256>);
        constexpr auto DIV512 = hana::make_pair(detail::AHB_PRE, integral_c<detail::HpreVal, detail::HpreVal::DIV512>);
    }

    namespace Apb1Prescaler
    {
        constexpr auto NO_DIV = hana::make_pair(detail::APB1_PRE, integral_c<detail::PpreVal, detail::PpreVal::NO_DIV>);
        constexpr auto DIV2   = hana::make_pair(detail::APB1_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV2>);
        constexpr auto DIV4   = hana::make_pair(detail::APB1_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV4>);
        constexpr auto DIV8   = hana::make_pair(detail::APB1_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV8>);
        constexpr auto DIV16  = hana::make_pair(detail::APB1_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV16>);
    }

    namespace Apb2Prescaler
    {
        constexpr auto NO_DIV = hana::make_pair(detail::APB2_PRE, integral_c<detail::PpreVal, detail::PpreVal::NO_DIV>);
        constexpr auto DIV2   = hana::make_pair(detail::APB2_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV2>);
        constexpr auto DIV4   = hana::make_pair(detail::APB2_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV4>);
        constexpr auto DIV8   = hana::make_pair(detail::APB2_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV8>);
        constexpr auto DIV16  = hana::make_pair(detail::APB2_PRE, integral_c<detail::PpreVal, detail::PpreVal::DIV16>);
    }

    template<detail::PllSrcVal source, std::uint32_t divide>
    constexpr auto pllInput(integral_constant<detail::PllSrcVal, source>, detail::ClockDivide<divide>)
    {
        return hana::make_pair(detail::PLL_SOURCE, detail::PllInputFactory<source, divide>{});
    }

    template<std::uint32_t mult, std::uint32_t div>
    constexpr auto i2sClock(detail::ClockMultiply<mult>, detail::ClockDivide<div>)
    {
        return hana::make_pair(detail::I2SCLK_SOURCE, detail::I2SPllConfigFactory<mult, div>{});
    }

    constexpr auto systemClock(detail::SysClkSrc<detail::SwVal::HSI> src)
    {
        return hana::make_pair(detail::SYSCLK_SOURCE, hana::make_pair(src, detail::VoidConfigFactory{}));
    }

    constexpr auto systemClock(detail::SysClkSrc<detail::SwVal::HSE> src)
    {
        return hana::make_pair(detail::SYSCLK_SOURCE, hana::make_pair(src, detail::VoidConfigFactory{}));
    }

    template<std::uint32_t mult, std::uint32_t div>
    constexpr auto systemClock(detail::SysClkSrc<detail::SwVal::PLL> src, detail::ClockMultiply<mult>, detail::ClockDivide<div>)
    {
        return hana::make_pair(detail::SYSCLK_SOURCE, hana::make_pair(src, detail::SystemPllConfigFactory<mult, div>{}));
    }
    
    template<class ... Options>
    constexpr auto makeClockConfig(Options && ... opts)
    {
        auto options = hana::make_map(std::forward<Options>(opts)...);
        
        static_assert(
            hana::is_subset(
                hana::keys(decltype(options){}),
                hana::make_basic_tuple(
                    detail::AHB_PRE,
                    detail::APB1_PRE,
                    detail::APB2_PRE,
                    detail::I2SCLK_SOURCE,
                    detail::PLL_SOURCE,
                    detail::SYSCLK_SOURCE
                )),
            "Invalid option supplied");

        using InputClks = detail::InputClocks<16000000, 8000000>;
        constexpr auto pllInputFactory = hana::find(options, detail::PLL_SOURCE)
            .value_or(detail::VoidConfigFactory{});
        using PllInput = decltype(pllInputFactory(std::declval<InputClks>()));

        constexpr auto sysClk_PllConfig = hana::find(options, detail::SYSCLK_SOURCE)
            .value_or(hana::make_pair(detail::SysClkSrc<detail::SwVal::HSI>{}, detail::VoidConfigFactory{}));
        constexpr auto swSrc = hana::first(sysClk_PllConfig);
        
        constexpr auto i2sPllFactory = hana::find(options, detail::I2SCLK_SOURCE)
            .value_or(detail::VoidConfigFactory{});

        using PllSystemConfig = 
            std::conditional_t<
                detail::IsVoidConfig<PllInput>::value,
                detail::VoidConfig,
                decltype(hana::second(sysClk_PllConfig)(std::declval<PllInput>()))>;
        using PllI2SConfig = 
            std::conditional_t<
                detail::IsVoidConfig<PllInput>::value,
                detail::VoidConfig,
                decltype(i2sPllFactory(std::declval<PllInput>()))>;
        using PllConfig = detail::PllConfig<PllInput, PllSystemConfig, PllI2SConfig>;

        constexpr auto ahbPrescaler = hana::find(options, detail::AHB_PRE).value_or(hana::second(AhbPrescaler::NO_DIV));
        constexpr auto apb1Prescaler = hana::find(options, detail::APB1_PRE).value_or(hana::second(Apb1Prescaler::NO_DIV));
        constexpr auto apb2Prescaler = hana::find(options, detail::APB2_PRE).value_or(hana::second(Apb2Prescaler::NO_DIV));

        return ClockConfig<
            InputClks, 
            hana::value(swSrc),
            hana::value(ahbPrescaler),
            hana::value(apb1Prescaler),
            hana::value(apb2Prescaler),
            PllConfig>{};
    }
}

}