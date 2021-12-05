#pragma once
#include "board/interrupts.hpp"

namespace drivers::spi::detail
{
    template<std::uint8_t deviceId>    
    constexpr auto getInterrupt(DeviceId<deviceId>)
    {
        static_assert(deviceId < 3, "Device id out of range");
        if constexpr(deviceId == 0)
        {
            return board::Interrupts::SPI1;
        }
        else if constexpr(deviceId == 1)
        {
            return board::Interrupts::SPI2;
        }
        else if constexpr(deviceId == 2)
        {
            return board::Interrupts::SPI3;
        }
    }
}