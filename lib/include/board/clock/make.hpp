#pragma once
#include "config.hpp"
#include <boost/hana/string.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/is_subset.hpp>

namespace board::clock
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

    using PllSource = rcc::PLLCFGR::PllSrcVal;
    using SystemClockSource = rcc::CFGR::SwVal;
    using AhbPrescaler = rcc::CFGR::HpreVal;
    using Apb1Prescaler = rcc::CFGR::PpreVal;
    using Apb2Prescaler = rcc::CFGR::PpreVal;

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

    struct InputClockConfig
    {
        std::uint32_t hsiClockFrequency = 16'000'000; 
        std::uint32_t hseClockFrequency = 8'000'000;
    };

    struct PllInputConfig
    {

    };

    struct SystemClockConfig
    {
        constexpr SystemClockConfig() : clockSource(SystemClockSource::HSI) { }

        constexpr Rational<std::uint32_t> getSystemClockFrequency(InputClockConfig inputClock) const
        {
            if (clockSource == SystemClockSource::HSI)
                return { inputClock.hsiClockFrequency };
            else if (clockSource == SystemClockSource::HSE)
                return { inputClock.hseClockFrequency };
        }

        SystemClockSource clockSource = SystemClockSource::HSI;
        union {
            const Rational<std::uint32_t> scale;
        };
    };

    namespace detail
    {
        constexpr std::uint32_t getAhbClockDivider(HpreVal ahbPreScaler)
        {
            if (ahbPreScaler == HpreVal::NO_DIV) return 1;
            else if (ahbPreScaler == HpreVal::DIV2) return 2;
            else if (ahbPreScaler == HpreVal::DIV4) return 4;
            else if (ahbPreScaler == HpreVal::DIV8) return 8;
            else if (ahbPreScaler == HpreVal::DIV16) return 16;
            else if (ahbPreScaler == HpreVal::DIV64) return 64;
            else if (ahbPreScaler == HpreVal::DIV128) return 128;
            else if (ahbPreScaler == HpreVal::DIV256) return 256;
            else if (ahbPreScaler == HpreVal::DIV512) return 512;
        }

        constexpr std::uint32_t getApbClockDivider(PpreVal apbPreScaler)
        {
            if (apbPreScaler == PpreVal::NO_DIV) return 1;
            else if (apbPreScaler == PpreVal::DIV2) return 2;
            else if (apbPreScaler == PpreVal::DIV4) return 4;
            else if (apbPreScaler == PpreVal::DIV8) return 8;
            else if (apbPreScaler == PpreVal::DIV16) return 16;
        }
    }

    struct ClockConfig
    {
        constexpr Rational<std::uint32_t> getSystemClockFrequency()
        {
            if (clockSource == SystemClockSource::HSI)
                return { inputClock.hsiClockFrequency };
            else if (clockSource == SystemClockSource::HSE)
                return { inputClock.hseClockFrequency };
        }

        static constexpr Rational<std::uint32_t> getAhbClockFrequency()
        {
            return getSystemClockFrequency() / detail::getAhbClockDivider(ahbPreScaler);
        }

        static constexpr Rational<std::uint32_t> getApb1ClockFrequency()
        {
            return ClockConfig::getAhbClockFrequency() / detail::getApbClockDivider(apb1PreScaler);
        }

        static constexpr Rational<std::uint32_t> getApb2ClockFrequency()
        {
            return ClockConfig::getAhbClockFrequency() / detail::getApbClockDivider(apb2PreScaler);
        }

        /*static constexpr Rational<std::uint32_t> getI2SClockFrequency()
        {
            return _PllConfig::getI2SClockFrequency();
        }*/

        InputClockConfig inputClock;
        SystemClockSource clockSource = SystemClockSource::HSI;
        AhbPrescaler ahbPreScaler = AhbPrescaler::NO_DIV;
        Apb1Prescaler apb1Prescaler = Apb1Prescaler::NO_DIV;
        Apb2Prescaler apb2Prescaler = Apb2Prescaler::NO_DIV;
    };

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