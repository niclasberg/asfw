#pragma once
#include "async/receiver.hpp"
#include "async/event.hpp"
#include "../spi_error.hpp"
#include "board/regmap/spi.hpp"
#include <cstdint>

#include "reg/unchecked_read.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::spi
{
    namespace detail
    {
        template<class SpiX, class DataType, class R>
        class ReadOperation : public async::EventHandlerImpl<ReadOperation<SpiX, DataType, R>>
        {
        public:
            template<class R2>
            ReadOperation(
                R2 && receiver, 
                const async::EventEmitter & interruptEvent, 
                DataType * data, 
                std::uint32_t size) 
            : receiver_(static_cast<R2&&>(receiver))
            , interruptEvent_(interruptEvent)
            , data_(data)
            , currentIndex_(0)
            , size_(size)
            {

            }

            void start()
            {
                if(size_ == 0)
                {
                    async::setValue(std::move(receiver_));
                    return;
                }

                if (!interruptEvent_.subscribe(this))
                {
                    async::setError(std::move(receiver_), SpiError::BUSY);
                    return;
                }

                reg::set(SpiX{}, board::spi::CR2::RXNEIE);
            }

            void handleEvent()
            {
                if(reg::bitIsSet(SpiX{}, board::spi::SR::RXNE))
                {
                    if (currentIndex_ < size_)
                    {
                        data_[currentIndex_++] = static_cast<std::uint8_t>(reg::uncheckedRead(SpiX{}, board::spi::DR::_Offset{}) & 0xFF);
                    }
                    else
                    {
                        // TODO Call this from the scheduler
                        stop();
                        async::setValue(std::move(receiver_));
                    }
                }
            }

            void stop()
            {
                reg::clear(SpiX{}, board::spi::CR2::RXNEIE);
                interruptEvent_.unsubscribe();
            }

        private:
            R receiver_;
            async::EventEmitter interruptEvent_;
            DataType * data_;
            std::uint32_t currentIndex_;
            const std::uint32_t size_;
        };
    }
}