#pragma once
#include "async/sender.hpp"
#include <concepts>
#include <cstdint>

namespace drivers::i2c
{
    template<class T>
    concept I2cLike = requires(T i2cDevice, std::uint8_t slaveAddress, const std::uint8_t * writeBuffer, std::uint8_t * readBuffer, std::uint8_t size)
        {
            {i2cDevice.read(slaveAddress, readBuffer, size)} -> async::Sender;
            {i2cDevice.write(slaveAddress, writeBuffer, size)} -> async::Sender;
            {i2cDevice.writeAndRead(slaveAddress, writeBuffer, size, readBuffer, size)} -> async::Sender;
        };
}