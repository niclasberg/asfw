#pragma once
#include "reg/field.hpp"
#include "peripheral.hpp"
#include "memory.hpp"
#include <array>
#include <cstdint>
#include <functional>

template<class Tag>
struct MockPeripheralControl
{
    static bool isEnabled;

    static void enable()
    {
        isEnabled = true;
    }

    static void disable()
    {
        isEnabled = false;
    }

    static void reset()
    {

    }
};

template<class Tag>
bool MockPeripheralControl<Tag>::isEnabled = false;

template<class Tag>
using MockPeripheral = Peripheral<Tag, MockDeviceMemory<Tag>, MockPeripheralControl<Tag>>;

// Helper methods for testing
template<class Tag>
void resetPeripheral(MockPeripheral<Tag>)
{
    MockDeviceMemory<Tag>::reset();
}

template<class Tag, class F>
void setOnWrite(MockPeripheral<Tag>, F && f)
{
    MockDeviceMemory<Tag>::onWrite = std::forward<F>(f);
}

template<class Tag, class F>
void setOnRead(MockPeripheral<Tag>, F && f)
{
    MockDeviceMemory<Tag>::onRead = std::forward<F>(f);
}

template<class Tag>
bool isEnabled(MockPeripheral<Tag>)
{
    return MockPeripheralControl<Tag>::isEnabled;
}

template<class Tag>
void fillDeviceMemory(MockPeripheral<Tag>, std::uint32_t value)
{
    MockDeviceMemory<Tag>::data.fill(value);
}

template<class Tag>
std::uint32_t getDeviceMemory(MockPeripheral<Tag>, std::uint32_t index)
{
    return *reinterpret_cast<std::uint32_t *>(
        reinterpret_cast<std::uint8_t *>(MockDeviceMemory<Tag>::data.data()) + index);
}

template<class Tag>
void setDeviceMemory(MockPeripheral<Tag>, std::uint32_t index, std::uint32_t value)
{
    *reinterpret_cast<std::uint32_t *>(
        reinterpret_cast<std::uint8_t *>(MockDeviceMemory<Tag>::data.data()) + index)
            = value;
}

// Bit operations which do not check for read/write permissions and does not cause
// calls to the onRead and onWrite callbacks in the MockDeviceMemory class
template<class Tag, class Location, class Mask, class RWPolicy, class ValueType>
void setRegisterBit(
    MockPeripheral<Tag>, 
    reg::Field<Location, Mask, RWPolicy, ValueType>)
{
    static_assert(reg::detail::isSingleBit<Mask>::value, "Only single bit fields can be set");
    constexpr auto addr = hana::value(typename Location::Offset{});
    MockDeviceMemory<Tag>::template getRef<addr>() |= (1 << Mask::shift);
}

template<class Tag, class Location, class Mask, class RWPolicy, class ValueType>
void clearRegisterBit(
    MockPeripheral<Tag>, 
    reg::Field<Location, Mask, RWPolicy, ValueType>)
{
    static_assert(reg::detail::isSingleBit<Mask>::value, "Only single bit fields can be cleared");
    constexpr auto addr = hana::value(typename Location::Offset{});
    MockDeviceMemory<Tag>::template getRef<addr>() &= ~(1 << Mask::shift);
}

template<class Tag, class Location, class Mask, class RWPolicy, class ValueType>
bool registerBitIsSet(
    MockPeripheral<Tag>, 
    reg::Field<Location, Mask, RWPolicy, ValueType>)
{
    static_assert(reg::detail::isSingleBit<Mask>::value, "Only single bit fields can be used for bitIsSet");
    constexpr auto addr = hana::value(typename Location::Offset{});
    return (MockDeviceMemory<Tag>::template getRef<addr>()) & (1 << Mask::shift);
}

template<class Tag, class Location, class Mask, class RWPolicy, class ValueType>
void setFieldValue(
    MockPeripheral<Tag>, 
    reg::Field<Location, Mask, RWPolicy, ValueType>, 
    ValueType value)
{
    constexpr auto addr = hana::value(typename Location::Offset{});
    MockDeviceMemory<Tag>::template getRef<addr>() &= ~(Mask::value);
    MockDeviceMemory<Tag>::template getRef<addr>() |= value << Mask::shift;
}

template<class Tag, class Location, class Mask, class RWPolicy, class ValueType>
ValueType getFieldValue(
    MockPeripheral<Tag>, 
    reg::Field<Location, Mask, RWPolicy, ValueType>)
{
    constexpr auto addr = hana::value(typename Location::Offset{});
    return ((MockDeviceMemory<Tag>::template getRef<addr>()) & Mask::type::value) >> Mask::shift;
}