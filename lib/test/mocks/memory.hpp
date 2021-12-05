#pragma once
#include <array>
#include <cstdint>
#include <functional>

template<class Tag>
struct MockDeviceMemory
{
    using OnWriteCallback = std::function<void (std::uint32_t, std::uint32_t)>;
    using OnReadCallback = std::function<void (std::uint32_t)>;

    static std::array<std::uint32_t, 256> data;
    static OnWriteCallback onWrite;
    static OnReadCallback onRead;

    using type = std::uint32_t;
    using ref_type = type &;
    using ptr_type = type *;

    static void reset()
    {
        data.fill(0);
        onWrite = nullptr;
        onRead = nullptr;
    }

    template<std::uint32_t offset>
    static std::uint32_t * getPtr()
    {
        return reinterpret_cast<std::uint32_t *>(
            reinterpret_cast<std::uint8_t *>(data.data()) + offset);
    }

    template<std::uint32_t offset>
    static std::uint32_t & getRef()
    {
        return *getPtr<offset>();
    }

    template<std::uint32_t offset>
    static void writeRegister(type value)
    {
        if(onWrite)
            onWrite(offset, value);
        *getPtr<offset>() = value;
    }

    template<std::uint32_t offset>
    static std::uint32_t readRegister()
    {
        if(onRead)
            onRead(offset);
        return *getPtr<offset>();
    }
};

template<class Tag>
std::array<std::uint32_t, 256> MockDeviceMemory<Tag>::data = {0};
template<class Tag>
typename MockDeviceMemory<Tag>::OnWriteCallback MockDeviceMemory<Tag>::onWrite = nullptr;
template<class Tag>
typename MockDeviceMemory<Tag>::OnReadCallback MockDeviceMemory<Tag>::onRead = nullptr;