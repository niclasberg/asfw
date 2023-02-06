#pragma once
#include "board/regmap/rcc.hpp"
#include "rational.hpp"

#include "reg/peripheral_operations.hpp"
#include "reg/bit_is_set.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/write.hpp"
#include "reg/read.hpp"

namespace board::clock
{
    namespace detail
    {
        using rcc::CFGR::PpreVal;
        using rcc::CFGR::HpreVal;
        using rcc::CFGR::SwVal;
        using rcc::PLLCFGR::PllSrcVal;
        using rcc::PLLCFGR::PllPVal;

        // Empty config, doing nothing
        struct VoidConfig
        {
            template<class ... Args>
            static void apply(Args && ...)
            {
                
            }
        };

        template<class T> struct IsVoidConfig : false_ {};
        template<> struct IsVoidConfig<VoidConfig> : true_ {};

        template<std::uint32_t hsiClockFrequency, std::uint32_t hseClockFrequency>
        struct InputClocks 
        { 
            static constexpr Rational<std::uint32_t> getHsiClockFrequency()
            {
                return Rational{hsiClockFrequency};
            }

            static constexpr Rational<std::uint32_t> getHseClockFrequency()
            {
                return Rational{hseClockFrequency};
            }
        };

        template<
            class InputClock,
            PllSrcVal pllSource,
            std::uint32_t sourceClockDivider>
        struct PllInputConfig
        {
            static_assert(sourceClockDivider >= 2 && sourceClockDivider <= 63,
                "The PLL source clock divider must be between 2 and 63");

            static constexpr Rational<std::uint32_t> getClockFrequency()
            {
                if constexpr (pllSource == PllSrcVal::HSE)
                {
                    return InputClock::getHseClockFrequency() / sourceClockDivider;
                }
                else
                {
                    return InputClock::getHsiClockFrequency() / sourceClockDivider;
                }
            }

            static_assert(
                    PllInputConfig::getClockFrequency() >= 1'000'000U,
					"PLL input clock frequency must be >= 1 MHz");
            static_assert(
					PllInputConfig::getClockFrequency() <= 2'000'000U,
					"PLL input clock frequency must be <= 2 MHz");

            template<class Rcc>
            static void apply(Rcc & rccDev)
            {
                // Enable the input clock generator
                if (pllSource == PllSrcVal::HSE && !reg::bitIsSet(rccDev, rcc::CR::HSEON))
                {
                    reg::set(rccDev, rcc::CR::HSEON);
                    while (!reg::bitIsSet(rccDev, rcc::CR::HSERDY)) { }
                }
                
                if (pllSource == PllSrcVal::HSI && !reg::bitIsSet(rccDev, rcc::CR::HSION))
                {
                    reg::set(rccDev, rcc::CR::HSION);
                    while (!reg::bitIsSet(rccDev, rcc::CR::HSIRDY)) { }
                }
                
                reg::write(rccDev, rcc::PLLCFGR::PLLSRC, pllSource);
                reg::write(rccDev, rcc::PLLCFGR::PLLM, uint32_c<sourceClockDivider>);
            }
        };

        template<
            class PllInput,
            std::uint32_t multiplier, 
            std::uint32_t divisor>
        struct I2SPllConfig
        {
            static_assert(bool_c<divisor >= 2> && bool_c<divisor <= 7>, "The Pll I2S divisor must be between 2 and 7");
            static_assert(bool_c<multiplier >= 50> && bool_c<multiplier <= 432>, "The Pll I2S multiplier must be between 50 and 432");

            static constexpr Rational<std::uint32_t> getClockFrequency()
            {
                return PllInput::getClockFrequency() * Rational<std::uint32_t>(multiplier, divisor);
            }

            static constexpr Rational<std::uint32_t> getVCOClockFrequency()
            {
                return PllInput::getClockFrequency() * multiplier;
            }

            static_assert(
                I2SPllConfig::getVCOClockFrequency() >= 100'000'000U,
                "I2S pll VCO frequency must be above 100 MHz (try increasing the multiplication factor");
            static_assert(
                I2SPllConfig::getVCOClockFrequency() <= 432'000'000U,
                "I2S pll VCO frequency must be below 432 MHz (try decreasing the multiplication factor");

            template<class Rcc>
            static void apply(Rcc & rccDev)
            {
                reg::write(rccDev, board::rcc::PLLI2SCFGR::PLLI2SNx, uint32_c<multiplier>);
                reg::write(rccDev, board::rcc::PLLI2SCFGR::PLLI2SRx, uint32_c<divisor>);

                // Enable and wait for clock to be ready
                reg::set(rccDev, board::rcc::CR::PLLI2SON);
                while (reg::read(rccDev, board::rcc::CR::PLLI2SRDY) == 0) {}
            }
        };

        template<std::uint32_t pllDivisor> struct PllDivisorToPllP;
        template<> struct PllDivisorToPllP<2> : integral_constant<PllPVal, PllPVal::DIV2> {};
        template<> struct PllDivisorToPllP<4> : integral_constant<PllPVal, PllPVal::DIV4> {};
        template<> struct PllDivisorToPllP<6> : integral_constant<PllPVal, PllPVal::DIV6> {};
        template<> struct PllDivisorToPllP<8> : integral_constant<PllPVal, PllPVal::DIV8> {};

        template<
            class PllInput, 
            std::uint32_t multiplier, 
            std::uint32_t divisor>
        struct SystemPllConfig
        {
            static_assert(bool_c<divisor == 2> || bool_c<divisor == 4> || bool_c<divisor == 6> || bool_c<divisor == 8>,
                "Invalid system PLL divisor, must be 2, 4, 6, or 8.");

            static constexpr auto getClockFrequency()
            {
                return PllInput::getClockFrequency() * Rational<std::uint32_t>(multiplier, divisor);
            }

            static constexpr auto getVCOClockFrequency()
            {
                return PllInput::getClockFrequency() * multiplier;
            }

            static_assert(
                SystemPllConfig::getVCOClockFrequency() >= 100'000'000U,
                "System pll VCO frequency must be above 100 MHz (try increasing the multiplication factor");
            static_assert(
                SystemPllConfig::getVCOClockFrequency() <= 432'000'000U,
                "System pll VCO frequency must be below 432 MHz (try decreasing the multiplication factor");

            template<class Rcc>
            static void apply(Rcc & rccDev)
            {
                reg::write(rccDev, rcc::PLLCFGR::PLLN, uint32_c<multiplier>);
                reg::write(rccDev, rcc::PLLCFGR::PLLP, PllDivisorToPllP<divisor>::value);
                
                // Enable Pll and wait for it to be ready
                reg::set(rccDev, rcc::CR::PLLON);
			    while (reg::read(rccDev, rcc::CR::PLLRDY) == 0) { }
            }
        };  

        template<
            class InputConfig,
            class SystemConfig,
            class I2SConfig>
        struct PllConfig
        {
            template<class T = SystemConfig>
            static constexpr 
            std::enable_if_t<!IsVoidConfig<T>::value, decltype(T::getClockFrequency())> 
            getSystemClockFrequency()
            {
                return T::getClockFrequency();
            }

            template<class T = SystemConfig>
            static constexpr std::enable_if_t<IsVoidConfig<T>::value, std::ratio<1>>
            getSystemClockFrequency()
            {
                // Just return a dummy value
                return {};
            }

            template<class T = I2SConfig>
            static constexpr 
            std::enable_if_t<!IsVoidConfig<T>::value, decltype(T::getClockFrequency())>
            getI2SClockFrequency()
            {
                return T::getClockFrequency();
            }

            template<class T = I2SConfig>
            static constexpr std::enable_if_t<IsVoidConfig<T>::value, std::ratio<1>>
            getI2SClockFrequency()
            {
                // Just return a dummy value
                return {};
            }

            template<class Rcc>
            static void apply(Rcc & rccDev)
            {
                InputConfig::template apply(rccDev);
                SystemConfig::template apply(rccDev);
                I2SConfig::template apply(rccDev);
            }
        };

        template<HpreVal ahbPreScaler>
        constexpr std::uint32_t getAhbClockDivider(constant_<ahbPreScaler>)
        {
            if constexpr (ahbPreScaler == HpreVal::NO_DIV) 
                return 1;
            else if constexpr (ahbPreScaler == HpreVal::DIV2)
                return 2;
            else if constexpr (ahbPreScaler == HpreVal::DIV4)
                return 4;
            else if constexpr (ahbPreScaler == HpreVal::DIV8)
                return 8;
            else if constexpr (ahbPreScaler == HpreVal::DIV16)
                return 16;
            else if constexpr (ahbPreScaler == HpreVal::DIV64)
                return 64;
            else if constexpr (ahbPreScaler == HpreVal::DIV128)
                return 128;
            else if constexpr (ahbPreScaler == HpreVal::DIV256)
                return 256;
            else if constexpr (ahbPreScaler == HpreVal::DIV512)
                return 512;
        }

        template<PpreVal apbPreScaler>
        constexpr std::uint32_t getApbClockDivider(constant_<apbPreScaler>)
        {
            if constexpr (apbPreScaler == PpreVal::NO_DIV) 
                return 1;
            else if constexpr (apbPreScaler == PpreVal::DIV2)
                return 2;
            else if constexpr (apbPreScaler == PpreVal::DIV4)
                return 4;
            else if constexpr (apbPreScaler == PpreVal::DIV8)
                return 8;
            else if constexpr (apbPreScaler == PpreVal::DIV16)
                return 16;
        }
    }

    template<
        class _InputClockFrequencies,
        detail::SwVal systemClockSource,
        detail::HpreVal ahbPreScaler,
        detail::PpreVal apb1PreScaler,
        detail::PpreVal apb2PreScaler,
        class _PllConfig>
    struct ClockConfig
    {
        static_assert(!(systemClockSource == detail::SwVal::PLL && detail::IsVoidConfig<_PllConfig>::value), 
            "A pll config must be supplied when the system clock source is set to PLL");

        static constexpr Rational<std::uint32_t> getSystemClockFrequency()
        {
            if constexpr (systemClockSource == detail::SwVal::HSI)
            {
                return _InputClockFrequencies::getHsiClockFrequency();
            }
            else if constexpr (systemClockSource == detail::SwVal::HSE)
            {
                return _InputClockFrequencies::getHseClockFrequency();
            }
            else
            {
                return _PllConfig::getSystemClockFrequency();
            }
        }

        static constexpr Rational<std::uint32_t> getAhbClockFrequency()
        {
            return ClockConfig::getSystemClockFrequency() / detail::getAhbClockDivider(constant_c<ahbPreScaler>);
        }

        static constexpr Rational<std::uint32_t> getApb1ClockFrequency()
        {
            return ClockConfig::getAhbClockFrequency() / detail::getApbClockDivider(constant_c<apb1PreScaler>);
        }

        static constexpr Rational<std::uint32_t> getApb2ClockFrequency()
        {
            return ClockConfig::getAhbClockFrequency() / detail::getApbClockDivider(constant_c<apb2PreScaler>);
        }

        static constexpr Rational<std::uint32_t> getI2SClockFrequency()
        {
            return _PllConfig::getI2SClockFrequency();
        }

        // Validate clock configuration
        static_assert(ClockConfig::getSystemClockFrequency() <= 168'000'000U,
                "System clock frequency must be below 168 MHz");
        static_assert(ClockConfig::getAhbClockFrequency() <= 168'000'000U,
                "AHB frequency must be below 168 MHz");
        static_assert(ClockConfig::getApb1ClockFrequency() <= 42'000'000U,
                "APB1 frequency must be below 42 MHz");
        static_assert(ClockConfig::getApb2ClockFrequency() <= 84'000'000U,
                "APB2 frequency must be below 84 MHz");

        template<class Rcc>
        static void apply(Rcc rccDev)
        {
            // Set maximal values for the APB prescalers to avoid an
            // invalid MCU state if we increase the clock frequency
            reg::write(rccDev, board::rcc::CFGR::PPRE1, detail::PpreVal::DIV16);
            reg::write(rccDev, board::rcc::CFGR::PPRE2, detail::PpreVal::DIV16);

            _PllConfig::template apply(rccDev);
            
            // Set system clock source, if not already set
            if (! reg::regHasValue(rccDev, board::rcc::CFGR::SWS, systemClockSource))
            {
                reg::write(rccDev, board::rcc::CFGR::SW, systemClockSource);

                // Wait until the change takes place
                while(!reg::regHasValue(rccDev, board::rcc::CFGR::SWS, systemClockSource)) { }
            }
            
            // Set prescaler values
            reg::write(rccDev, board::rcc::CFGR::HPRE, ahbPreScaler);
            reg::write(rccDev, board::rcc::CFGR::PPRE1, apb1PreScaler);
            reg::write(rccDev, board::rcc::CFGR::PPRE2, apb2PreScaler);
        }
    };
}
