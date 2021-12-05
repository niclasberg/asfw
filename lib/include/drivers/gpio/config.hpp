#pragma once
#include "types.hpp"
#include "board/regmap/gpio.hpp"
#include "board/regmap/exti.hpp"
#include "board/regmap/syscfg.hpp"
#include "board/interrupts.hpp"

#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/write.hpp"

namespace drivers
{

namespace gpio
{
    namespace detail
    {
        using board::gpio::MODER::ModerVal;
        using board::gpio::PUPDR::PupdrVal;
        using board::gpio::OSPEEDR::OspeedrVal;
        using board::gpio::OTYPER::OtyperVal;
        using board::gpio::AFR::AfrVal;

        template<
            std::uint8_t port, 
            std::uint8_t pin, 
            ModerVal mode>
        struct GpioConfigBase
        {
            template<class GpioX>
            static void apply(GpioX gpioX)
            {
                gpioX.enable();
                reg::write(gpioX, board::gpio::MODER::MODER[uint8_c<pin>], ENUM_C(mode));
            }
        };

        template<std::uint8_t port, std::uint8_t pin, PupdrVal pupd>
        struct InputPinConfig
        {
            template<class GpioX>
            static void apply(GpioX gpioX)
            {
                GpioConfigBase<port, pin, ModerVal::INPUT>::apply(gpioX);
                reg::write(gpioX, board::gpio::PUPDR::PUPDR[uint8_c<pin>], pupd);
            }
        };

        template<std::uint8_t port, std::uint8_t pin, OspeedrVal speed, PupdrVal pupd, OtyperVal outType>
        struct OutputPinConfig
        {
            template<class GpioX>
            static void apply(GpioX gpioX)
            {
            	GpioConfigBase<port, pin, ModerVal::OUTPUT>::apply(gpioX);
                reg::write(gpioX, board::gpio::OSPEEDR::OSPEEDR[uint8_c<pin>], ENUM_C(speed));
                reg::write(gpioX, board::gpio::PUPDR::PUPDR[uint8_c<pin>], ENUM_C(pupd));
                reg::write(gpioX, board::gpio::OTYPER::OT[uint8_c<pin>], ENUM_C(outType));
            }
        };

        template<std::uint8_t port, std::uint8_t pin>
        struct AnalogPinConfig
        {
            template<class GpioX>
            static void apply(GpioX gpioX)
            {
            	GpioConfigBase<port, pin, ModerVal::ANALOG>::apply(gpioX);
            }
        };
        
        template<std::uint8_t port, std::uint8_t pin, AfrVal altFn, OspeedrVal speed, PupdrVal pupd, OtyperVal outType>
        struct AltFnPinConfig
        {
            template<class GpioX>
            static void apply(GpioX gpioX)
            {
            	GpioConfigBase<port, pin, ModerVal::ALTERNATE_FUNCTION>::apply(gpioX);
                reg::write(gpioX, board::gpio::OSPEEDR::OSPEEDR[uint8_c<pin>], ENUM_C(speed));
                reg::write(gpioX, board::gpio::PUPDR::PUPDR[uint8_c<pin>], ENUM_C(pupd));
                reg::write(gpioX, board::gpio::OTYPER::OT[uint8_c<pin>], ENUM_C(outType));
                reg::write(gpioX, board::gpio::AFR::AFR[uint8_c<pin>], ENUM_C(altFn));
            }
        };

        struct InterruptBaseConfig
        {
            template<class Exti, class SysCfg, std::uint8_t port, std::uint8_t pin>
            static void apply(Exti, SysCfg syscfg, uint8_<port>, uint8_<pin>)
            {
                syscfg.enable();
                reg::write(syscfg, board::syscfg::EXTICR::EXTI[uint8_c<pin>], uint32_c<port>);
            }
        };

        struct RisingEdgeInterrupt
        {
            template<class Exti, class SysCfg, std::uint8_t port, std::uint8_t pin>
            static void apply(Exti exti, SysCfg, uint8_<port>, uint8_<pin>)
            { 
                reg::set(exti, board::exti::RTSR::TR[uint8_c<pin>]);
                reg::clear(exti, board::exti::FTSR::TR[uint8_c<pin>]);
            }
        };

        struct FallingEdgeInterrupt
        {
            template<class Exti, class SysCfg, std::uint8_t port, std::uint8_t pin>
            static void apply(Exti exti, SysCfg, uint8_<port>, uint8_<pin>)
            { 
                reg::clear(exti, board::exti::RTSR::TR[uint8_c<pin>]);
                reg::set(exti, board::exti::FTSR::TR[uint8_c<pin>]);
            }
        };

        struct RisingFallingEdgeInterrupt
        {
            template<class Exti, class SysCfg, std::uint8_t port, std::uint8_t pin>
            static void apply(Exti exti, SysCfg, uint8_<port>, uint8_<pin>)
            { 
                reg::set(exti, board::exti::RTSR::TR[uint8_c<pin>]);
                reg::set(exti, board::exti::FTSR::TR[uint8_c<pin>]);
            }
        };
    }
}

}
