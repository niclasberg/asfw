#pragma once
#include "types.hpp"

namespace PeripheralTypes
{
    namespace tags
    {
        template<std::uint8_t portNo> struct Gpio {};
        template<std::uint8_t id> struct I2c {};
        template<std::uint8_t id> struct Spi {};
        template<std::uint8_t id> struct Uart {};
        template<std::uint8_t id> struct Dma {};
        template<std::uint8_t id> struct Adc {};
        struct Exti {};
        struct SysCfg {};
    }

    template<std::uint8_t portNo> constexpr tags::Gpio<portNo> GPIO{};
    template<std::uint8_t id> constexpr tags::I2c<id> I2C{};
    template<std::uint8_t id> constexpr tags::Spi<id> SPI{};
    template<std::uint8_t id> constexpr tags::Uart<id> UART{};
    template<std::uint8_t id> constexpr tags::Dma<id> DMA{};
    template<std::uint8_t id> constexpr tags::Adc<id> ADC{};
    constexpr tags::Exti EXTI{};
    constexpr tags::SysCfg SYSCFG{};
}