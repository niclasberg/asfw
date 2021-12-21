#pragma once
#include "i2c_common.hpp"
#include "reg/unchecked_write.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX, bool restartAfterComplete = false>
    class ReadEventHandler
    {
    public:
        ReadEventHandler(std::uint8_t * data, std::uint16_t size)
        : data_(data)
        , size_(size)
        , bytesRemaining_(size)
        {

        }

        template<class Continuation>
        void operator()(Continuation & continuation, std::uint8_t slaveAddr) volatile
        {
            auto sr1val = reg::getRegisterValue(I2cX{}, board::i2c::SR1::_Offset{});
            if (reg::bitIsSet(sr1val, board::i2c::SR1::SB))
            {
                // Clear start bit by reading SR1 followed by writing the slave address to DR
                uint32_t dummy = reg::uncheckedRead(I2cX{}, board::i2c::SR1::_Offset{});
                reg::uncheckedWrite(I2cX{}, board::i2c::DR::_Offset{}, ((slaveAddr << 1) | 1));
                (void)dummy;
            }
            else if (reg::bitIsSet(sr1val, board::i2c::SR1::ADDR))
            {
                if (size_ == 0)
                {
                    clearAddressBit<I2cX>();
                    sendStopOrRestartCondition<I2cX, restartAfterComplete>();
                    continuation.finishTransactionWithValue();
                    return;
                }
                else if (size_ == 1)
                {
                    // NACK the next incoming byte
                    reg::clear(I2cX{}, board::i2c::CR1::POS);
                    reg::clear(I2cX{}, board::i2c::CR1::ACK);
                    clearAddressBit<I2cX>();
                }
                else if (size_ == 2)
                {
                    // NACK the second next incoming byte
                    reg::set(I2cX{}, board::i2c::CR1::POS);
                    reg::clear(I2cX{}, board::i2c::CR1::ACK);
                    clearAddressBit<I2cX>();
                }
                else
                {
                    // Enable ACKing
                    reg::clear(I2cX{}, board::i2c::CR1::POS);
                    reg::set(I2cX{}, board::i2c::CR1::ACK);
                    clearAddressBit<I2cX>();
                }

                // Enable interrupt on TxE and RxNE events.
                // Edge case: while reading, for 2 or 3 bytes we wait for BTF instead
                if (size_ == 1 || size_ > 3)
                {
                    reg::set(I2cX{}, board::i2c::CR2::ITBUFEN);
                }
            }
            else if (reg::bitIsSet(sr1val, board::i2c::SR1::RxNE))
            {
                if (size_ == 1)
                {
                    sendStopOrRestartCondition<I2cX, restartAfterComplete>();
                    readByte();
                    continuation.finishTransactionWithValue();
                    return;
                }
                else if (bytesRemaining_ > 3)
                {
                    readByte();
                }

                // We no longer rely on RxNE when buffer size is less than 4
                if (bytesRemaining_ <= 3)
                {
                    reg::clear(I2cX{}, board::i2c::CR2::ITBUFEN);
                }
            }

            if (reg::bitIsSet(I2cX{}, board::i2c::SR1::BTF))
            {
                if (bytesRemaining_ == 3)
                {
                    reg::clear(I2cX{}, board::i2c::CR1::ACK);
                    readByte();
                }
                else if (bytesRemaining_ == 2)
                {
                    // Once BTF is set, the data register and the shift register
                    // each contain a byte. Generate a stop/restart and receive the bytes.
                    sendStopOrRestartCondition<I2cX, restartAfterComplete>();
                    readByte();
                    readByte();

                    // The transfer is finished at this point
                    continuation.finishTransactionWithValue();
                    return;
                }
            }
        }

        void readByte() volatile
        {
            *data_ = reg::uncheckedRead(I2cX{}, board::i2c::DR::_Offset{}) & 0xFF;
            data_ = data_ + 1;
            bytesRemaining_ = bytesRemaining_ - 1;
        }

    private:
        volatile std::uint8_t * volatile data_;
        const std::uint16_t size_;
        volatile std::uint16_t bytesRemaining_;
    };
}