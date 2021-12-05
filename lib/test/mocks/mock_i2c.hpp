#pragma once
#include "async/just.hpp"

struct MockI2c
{
    auto read(std::uint8_t slaveAddress, std::uint8_t * , std::uint16_t)
    {
        readSlaveAddress = slaveAddress;
        return async::justValue();
    } 

    auto write(std::uint8_t slaveAddress, const std::uint8_t * bytesToWrite, std::uint16_t size)
    {
        writeSlaveAddress = slaveAddress;
        bytesWritten.insert(bytesWritten.end(), bytesToWrite, bytesToWrite+size);
        return async::justValue();
    }

    auto writeAndRead(std::uint8_t slaveAddress, const std::uint8_t *, std::uint16_t, std::uint8_t *, std::uint16_t)
    {
        readSlaveAddress = slaveAddress;
        writeSlaveAddress = slaveAddress;
        return async::justValue();
    }

    std::uint8_t readSlaveAddress = 0x00;
    std::uint8_t writeSlaveAddress = 0x00;
    std::vector<std::uint8_t> bytesWritten = {};
    std::vector<std::uint8_t> readBuffer = {};
};