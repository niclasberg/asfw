#pragma once
#include "types.hpp"
#include "reg/write.hpp"
#include "reg/toggle.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"

namespace drivers::gpio
{
    template<class GpioX, std::uint8_t portNo, std::uint8_t pinNo>
    struct OutputPin
    {
        void write(bool value)
        {
            reg::write(GpioX{}, board::gpio::ODR::ODR[uint8_c<pinNo>], value);
        }

        void toggle()
        {
            reg::toggle(GpioX{}, board::gpio::ODR::ODR[uint8_c<pinNo>]);
        }

        void set()
        {
            reg::set(GpioX{}, board::gpio::ODR::ODR[uint8_c<pinNo>]);
        }

        void clear()
        {
            reg::clear(GpioX{}, board::gpio::ODR::ODR[uint8_c<pinNo>]);
        }
    };
}