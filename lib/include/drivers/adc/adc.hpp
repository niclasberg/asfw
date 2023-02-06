#pragma once
#include "async/future.hpp"
#include "async/make_future.hpp"
#include "async/receiver.hpp"
#include "adc_error.hpp"
#include "async/scheduler.hpp"
#include "async/event.hpp"
#include "drivers/dma/dma_concepts.hpp"

#include "reg/apply.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"

namespace drivers::adc
{
    template<class AdcX, std::uint8_t NChannels, std::uint16_t _maxValue>
    class Adc
    {
        template<class TransferFactory, class R>
        class ReadDmaOperation
        {
            struct DmaEventHandler
            {
                void operator()(dma::DmaSignal signal)
                {
                    switch (signal)
                    {
                        case dma::DmaSignal::TRANSFER_COMPLETE:
                            op_.setValue();
                            break;
                        default:
                            break;
                    }
                }

                ReadDmaOperation & op_;
            };

            using DmaTransfer = dma::DmaTransferType<TransferFactory, DmaEventHandler>;
        public:
            template<class TransferFactory2, class R2>
            ReadDmaOperation(TransferFactory2 && transferFactory, R2 && receiver)
            : transfer_(static_cast<TransferFactory2&&>(transferFactory)(DmaEventHandler{*this}))
            , receiver_(static_cast<R2&&>(receiver))
            {

            }

            void start()
            {
                if (!transfer_.start())
                {
                    async::setError(std::move(receiver_), AdcError::BUSY);
                    return;
                }

                reg::apply(AdcX{},
                    reg::clear(board::adc::SR::EOC),
                    reg::clear(board::adc::SR::OVR));

                reg::set(AdcX{}, board::adc::CR2::DMA);
                reg::set(AdcX{}, board::adc::CR2::SWSTART);
            }

            void stop()
            {

            }

        private:
            void setValue()
            {
                auto & s = async::getScheduler(receiver_);
                s.postFromISR({memFn<&ReadDmaOperation::setValueImpl>, *this});
            }

            void setValueImpl()
            {
                reg::clear(AdcX{}, board::adc::CR2::DMA);
                transfer_.stop();
                async::setValue(std::move(receiver_));
            }

            DmaTransfer transfer_;
            R receiver_;
        };

    public:
        explicit Adc(const async::EventEmitter & eventEmitter)
        : eventEmitter_(eventEmitter)
        {
            
        }

        inline static constexpr std::uint8_t numberOfChannels = NChannels;
        inline static constexpr std::uint16_t maxValue = _maxValue;

        template<dma::DmaLike Dma>
        async::Future<void, AdcError> auto read(Dma & dmaDevice, std::uint16_t * buffer)
        {
            auto transferFactory = dmaDevice.transferSingle(
                dma::PeripheralAddress(AdcX{}.getAddress(board::adc::DR::_Offset{})),
                dma::MemoryAddress(reinterpret_cast<std::uint32_t>(buffer)),
                NChannels);
            using TransferFactoryType = decltype(transferFactory);
            
            return async::makeFuture<void, AdcError>(
                [this, transferFactory]<typename R>(R && receiver) mutable
                    -> ReadDmaOperation<TransferFactoryType, std::remove_cvref_t<R>> 
                {
                    return {std::move(transferFactory), static_cast<R&&>(receiver)};
                });
        }
    private:
        async::EventEmitter eventEmitter_;
    };
}