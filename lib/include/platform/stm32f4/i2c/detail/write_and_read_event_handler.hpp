#pragma once
#include "read_event_handler.hpp"
#include "write_event_handler.hpp"
#include "../i2c_error.hpp"

namespace drivers::i2c::detail
{
    template<class I2cX>
    class WriteAndReadEventHandler
    {
        enum class State
        {
            WRITING,
            READING
        };

        struct FirstContinuation
        {
            void setValue()
            {
                parent_.state_ = State::READING;
            }

            void setError(I2cError error)
            {
                parent_.receiver_.setError(error);
            }

            WriteAndReadEventHandler & parent_;
        };
    
    public:
        WriteAndReadEventHandler(
            const std::uint8_t * writeData,
            std::uint16_t writeSize,
            std::uint8_t * readBuffer,
            std::uint16_t readSize)
        : state_(State::WRITING)
        , writeHandler_(writeData, writeSize)
        , readHandler_(readBuffer, readSize)
        {

        }
    
        void finishTransactionWithValue() volatile
        {
            // This will be called when the writeHandler is finished
            state_ = State::READING;
        }

        template<class Continuation>
        void operator()(Continuation & continuation, std::uint8_t slaveAddr) volatile
        {
            switch (state_)
            {
                case State::WRITING:
                    writeHandler_(*this, slaveAddr);
                    break;
                case State::READING:
                    readHandler_(continuation, slaveAddr);
                    break;
            }
        }

    private:
        volatile State state_;
        // It would be possible to use a union here, which would
        // save 2 bytes. This would however complicate the code 
        // since the readHandler would need to be lazily constructed.
        WriteEventHandler<I2cX, true> writeHandler_; // true -> generate start condition (instead of stop) after completion
        ReadEventHandler<I2cX, false> readHandler_;
    };
}