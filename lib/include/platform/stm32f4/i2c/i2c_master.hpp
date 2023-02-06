#pragma once
#include "async/event.hpp"
#include "detail/write_sender.hpp"
#include "detail/read_sender.hpp"
#include "detail/write_and_read_sender.hpp"

namespace drivers::i2c
{
    template<class I2cX>
    class I2cMaster
    {
    public:
        I2cMaster(const async::EventEmitter & evtInterrupt, const async::EventEmitter & errInterrupt)
        : eventInterrupt_(evtInterrupt), errorInterrupt_(errInterrupt)
        {

        }

        I2cMaster(const I2cMaster &) = delete;
        I2cMaster(I2cMaster &&) = delete;
        I2cMaster & operator=(const I2cMaster &) = delete;
        I2cMaster & operator=(I2cMaster &&) = delete;

        async::Future<void, I2cError> auto write(std::uint8_t slaveAddress, const std::uint8_t * data, std::uint16_t size)
            -> detail::WriteSender<I2cX>
        {
            return {slaveAddress, eventInterrupt_, errorInterrupt_, data, size};
        }

        async::Future<void, I2cError> auto read(std::uint8_t slaveAddress, std::uint8_t * data, std::uint16_t size)
            -> detail::ReadSender<I2cX>
        {
            return {slaveAddress, eventInterrupt_, errorInterrupt_, data, size};
        }

        async::Future<void, I2cError> auto writeAndRead(std::uint8_t slaveAddress, const std::uint8_t * writeData, std::uint16_t writeSize, std::uint8_t * readBuffer, std::uint16_t readSize)
            -> detail::WriteAndReadSender<I2cX>
        {
            return {slaveAddress, eventInterrupt_, errorInterrupt_, writeData, writeSize, readBuffer, readSize};
        }

    private:
        async::EventEmitter eventInterrupt_; 
        async::EventEmitter errorInterrupt_;
    };
}