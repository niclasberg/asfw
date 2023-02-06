#pragma once
#include "i2c_common.hpp"
#include "reg/peripheral_operations.hpp"
#include "board/regmap/i2c.hpp"
#include "reg/unchecked_write.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX, bool restartAfterComplete = false>
    class WriteEventHandler
    {
    public:
        WriteEventHandler(const std::uint8_t * data, std::uint16_t size)
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
                //uint32_t dummy = reg::uncheckedRead(I2cX{}, board::i2c::SR1::_Offset{});
                reg::uncheckedWrite(I2cX{}, board::i2c::DR::_Offset{}, ((slaveAddr << 1) & ~1));
                //(void)dummy;
            }
            else if (reg::bitIsSet(sr1val, board::i2c::SR1::ADDR))
            {
                clearAddressBit<I2cX>();
                reg::set(I2cX{}, board::i2c::CR2::ITBUFEN);
            }
            else if (reg::bitIsSet(sr1val, board::i2c::SR1::TxE))
            {
                if (bytesRemaining_ != 0)
                {
                    reg::uncheckedWrite(I2cX{}, board::i2c::DR::_Offset{}, *data_);
                    data_ = data_ + 1;
                    bytesRemaining_ = bytesRemaining_ - 1;
                }

                if (bytesRemaining_ == 0)
                {
                    reg::clear(I2cX{}, board::i2c::CR2::ITBUFEN);
                }
            }

            if (reg::bitIsSet(I2cX{}, board::i2c::SR1::BTF))
            {
                if (bytesRemaining_ == 0)
                {
                    sendStopOrRestartCondition<I2cX, restartAfterComplete>();
                    continuation.finishTransactionWithValue();
                    return;
                }
            }
        }

    private:
        const std::uint8_t * volatile data_;
        const std::uint16_t size_;
        volatile std::uint16_t bytesRemaining_;
    };   
}