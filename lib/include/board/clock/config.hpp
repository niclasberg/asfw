#pragma once
#include "board/regmap/rcc.hpp"
#include "cratio.hpp"
#include "reg/peripheral_operations.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/write.hpp"
#include "reg/read.hpp"

namespace board
{

namespace clock
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
            static constexpr auto getHsiClockFrequency() -> std::ratio<hsiClockFrequency>
            {
                return {};
            }

            static constexpr auto getHseClockFrequency() -> std::ratio<hseClockFrequency>
            {
                return {};
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

            static constexpr auto getClockFrequency()
            {
                if constexpr (pllSource == PllSrcVal::HSE)
                {
                    return hana::mult(std::ratio<1, sourceClockDivider>{}, InputClock::getHseClockFrequency());
                }
                else
                {
                    return hana::mult(std::ratio<1, sourceClockDivider>{}, InputClock::getHsiClockFrequency());
                }
            }

            static_assert(
                    hana::greater_equal(PllInputConfig::getClockFrequency(), std::ratio<1000000>{}),
					"PLL input clock frequency must be >= 1 MHz");
            static_assert(
					hana::less_equal(PllInputConfig::getClockFrequency(), std::ratio<2000000>{}),
					"PLL input clock frequency must be <= 2 MHz");

            template<class Rcc>
            static void apply(Rcc & rccDev)
            {
                // Enable the input clock generator
                if(pllSource == PllSrcVal::HSE && !bitIsSet(rccDev, rcc::CR::HSEON))
                {
                    reg::set(rccDev, rcc::CR::HSEON);
                    while (!reg::bitIsSet(rccDev, rcc::CR::HSERDY)) { }
                }
                if(pllSource == PllSrcVal::HSI && !bitIsSet(rccDev, rcc::CR::HSION))
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

            static constexpr auto getClockFrequency()
            {
                return hana::mult(PllInput::getClockFrequency(), std::ratio<multiplier, divisor>{});
            }

            static constexpr auto getVCOClockFrequency()
            {
                return hana::mult(PllInput::getClockFrequency(), std::ratio<multiplier>{});
            }

            static_assert(
                hana::greater_equal(I2SPllConfig::getVCOClockFrequency(), std::ratio<100000000>{}),
                "I2S pll VCO frequency must be above 100 MHz (try increasing the multiplication factor");
            static_assert(
                hana::less_equal(I2SPllConfig::getVCOClockFrequency(), std::ratio<432000000>{}),
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
                return hana::mult(PllInput::getClockFrequency(), std::ratio<multiplier, divisor>{});
            }

            static constexpr auto getVCOClockFrequency()
            {
                return hana::mult(PllInput::getClockFrequency(), std::ratio<multiplier>{});
            }

            static_assert(
                hana::greater_equal(SystemPllConfig::getVCOClockFrequency(), std::ratio<100000000>{}),
                "System pll VCO frequency must be above 100 MHz (try increasing the multiplication factor");
            static_assert(
                hana::less_equal(SystemPllConfig::getVCOClockFrequency(), std::ratio<432000000>{}),
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
                return {};
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
                return {};
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

        template<HpreVal hpre> struct AhbPrescalerValue;
        template<> struct AhbPrescalerValue<HpreVal::NO_DIV> : hana::type<std::ratio<1>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV2> : hana::type<std::ratio<1, 2>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV4> : hana::type<std::ratio<1, 4>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV8> : hana::type<std::ratio<1, 8>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV16> : hana::type<std::ratio<1, 16>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV64> : hana::type<std::ratio<1, 64>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV128> : hana::type<std::ratio<1, 128>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV256> : hana::type<std::ratio<1, 256>> {};
        template<> struct AhbPrescalerValue<HpreVal::DIV512> : hana::type<std::ratio<1, 512>> {};

        template<HpreVal hpre>
        constexpr auto ahbPrescalerValue = typename AhbPrescalerValue<hpre>::type{};

        template<PpreVal ppre> struct ApbPrescalerValue;
        template<> struct ApbPrescalerValue<PpreVal::NO_DIV> : std::ratio<1> {};
        template<> struct ApbPrescalerValue<PpreVal::DIV2> : std::ratio<1, 2> {};
        template<> struct ApbPrescalerValue<PpreVal::DIV4> : std::ratio<1, 4> {};
        template<> struct ApbPrescalerValue<PpreVal::DIV8> : std::ratio<1, 8> {};
        template<> struct ApbPrescalerValue<PpreVal::DIV16> : std::ratio<1, 16> {};

        template<PpreVal ppre>
        constexpr auto apbPrescalerValue = typename ApbPrescalerValue<ppre>::type{};
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

        static constexpr auto getSystemClockFrequency()
        {
            return hana::if_(
                bool_c<systemClockSource == detail::SwVal::HSI>,
                _InputClockFrequencies::getHsiClockFrequency(),
                hana::if_(
                    bool_c<systemClockSource == detail::SwVal::HSE>,
                    _InputClockFrequencies::getHseClockFrequency(),
                    _PllConfig::getSystemClockFrequency()));
        }

        static constexpr auto getAhbClockFrequency()
        {
            return hana::mult(ClockConfig::getSystemClockFrequency(), detail::ahbPrescalerValue<ahbPreScaler>);
        }

        static constexpr auto getApb1ClockFrequency()
        {
            return hana::mult(ClockConfig::getAhbClockFrequency(), detail::apbPrescalerValue<apb1PreScaler>);
        }

        static constexpr auto getApb2ClockFrequency()
        {
            return hana::mult(ClockConfig::getAhbClockFrequency(), detail::apbPrescalerValue<apb2PreScaler>);
        }

        static constexpr auto getI2SClockFrequency()
        {
            return _PllConfig::getI2SClockFrequency();
        }

        // Validate clock configuration
        static_assert(hana::less_equal(ClockConfig::getSystemClockFrequency(), std::ratio<168000000>{}),
                "System clock frequency must be below 168 MHz");
        static_assert(hana::less_equal(ClockConfig::getAhbClockFrequency(), std::ratio<168000000>{}),
                "AHB frequency must be below 168 MHz");
        static_assert(hana::less_equal(ClockConfig::getApb1ClockFrequency(), std::ratio<42000000>{}),
                "APB1 frequency must be below 42 MHz");
        static_assert(hana::less_equal(ClockConfig::getApb2ClockFrequency(), std::ratio<84000000>{}),
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

}
