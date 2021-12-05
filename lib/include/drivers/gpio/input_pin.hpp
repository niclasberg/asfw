#pragma once
#include "types.hpp"
#include "reg/read.hpp"

namespace drivers::gpio
{
    template<class GpioX, std::uint8_t portNo, std::uint8_t pinNo>
    struct InputPin
    {
        bool read()
		{
			return reg::read(GpioX{}, board::gpio::IDR::IDR[uint8_c<pinNo>]);
		}
    };
}