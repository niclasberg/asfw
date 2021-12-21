#pragma once
#include <cstdint>

namespace drivers::dma
{
    class PeripheralAddress
    {
    public:
    	PeripheralAddress(std::uint32_t address) : address_(address) { }

        constexpr std::uint32_t getAddress() const { return address_; }
    private:
        std::uint32_t address_;
    };

    class MemoryAddress
    {
    public:
    	constexpr MemoryAddress(std::uint32_t address) : address_(address) { }
        constexpr std::uint32_t getAddress() const { return address_; }

    private:
        std::uint32_t address_;
    };

    class MemoryAddressPair
    {
    public:
        constexpr MemoryAddressPair(std::uint32_t address0, std::uint32_t address1)
        : address0_(address0), address1_(address1) { }

        constexpr std::uint32_t getAddress0() const { return address0_; }
        constexpr std::uint32_t getAddress1() const { return address1_; }

    private:
        std::uint32_t address0_;
        std::uint32_t address1_;
    };
}