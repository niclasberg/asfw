#pragma once
#include <cstdint>
#include "async/future.hpp"

namespace asfw::platform
{
    enum class I2cError
    {
        BUSY,
        ACKNOWLEDGE_FAILURE,
        ARBITRATION_LOST,
        BUS_ERROR,
        UNKNOWN
    };

    template<class T>
    concept I2cMaster = requires(T i2cDevice, std::uint8_t slaveAddress, const std::uint8_t * writeBuffer, std::uint8_t * readBuffer, std::uint8_t size)
    {
        {i2cDevice.read(slaveAddress, readBuffer, size)} -> async::Future<void, I2cError>;
        {i2cDevice.write(slaveAddress, writeBuffer, size)} -> async::Future<void, I2cError>;
        {i2cDevice.writeAndRead(slaveAddress, writeBuffer, size, readBuffer, size)} -> async::Future<void, I2cError>;
    };
}