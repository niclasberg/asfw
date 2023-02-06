#pragma once
#include "../i2c_error.hpp"
#include "board/regmap/i2c.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class ErrorEventHandler
    {
    public:
        template<class Continuation>
        void operator()(Continuation & continuation) volatile
        {
            auto acknowledgeFailure = reg::bitIsSet(I2cX{}, board::i2c::SR1::AF);
            if(acknowledgeFailure)
            {
                reg::set(I2cX{}, board::i2c::CR1::STOP);
                continuation.finishTransactionWithError(I2cError::ACKNOWLEDGE_FAILURE);
                return;
            }
        }
    };
}