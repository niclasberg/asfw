#pragma once
#include <array>
#include "async/receiver.hpp"
#include "async/make_future.hpp"
#include "async/scheduler.hpp"
#include "drivers/dma/dma_concepts.hpp"
#include "drivers/dma/dma_signal.hpp"
#include "../spi_error.hpp"
#include "delegate.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::spi::detail
{
    template<class SpiX, class DataType, class TransferFactory, class R, class F>
    class WriteDmaOperation
    {
        struct DmaEventHandler
        {
            void operator()(dma::DmaSignal signal)
            {
                auto & s = async::getScheduler(op_.receiver_);
                switch (signal)
                {
                    case dma::DmaSignal::TRANSFER_COMPLETE:
                        s.postFromISR({memFn<&WriteDmaOperation::fillBuffer0>, op_});
                        break;
                    case dma::DmaSignal::TRANSFER_COMPLETE_MEMORY1:
                        s.postFromISR({memFn<&WriteDmaOperation::fillBuffer1>, op_});
                        break;
                    default:
                        break;
                }
            }

            WriteDmaOperation & op_;
        };

        using DmaTransfer = dma::DmaTransferType<TransferFactory, DmaEventHandler>;
    public:
        template<class TransferFactory2, class R2>
        WriteDmaOperation(TransferFactory2 && transferFactory, R2 && receiver, DataType * buffer0, DataType * buffer1)
        : transfer_(static_cast<TransferFactory2&&>(transferFactory)(DmaEventHandler{*this}))
        , receiver_(static_cast<R2&&>(receiver))
        , buffer0_{buffer0}
        , buffer1_{buffer1}
        {
            
        }

        void start()
        {
            if (!transfer_.start())
            {
                async::setError(std::move(receiver_), SpiError::BUSY);
                return;
            }

            reg::set(SpiX{}, board::spi::CR2::TXDMAEN);
        }

        void stop()
        {
            reg::clear(SpiX{}, board::spi::CR2::TXDMAEN);
            transfer_.stop();
            async::setDone(std::move(receiver_));
        }

    private:
        void fillBuffer0()
        {
            callback_(buffer0_);
        }

        void fillBuffer1()
        {
            callback_(buffer0_);
        }

        DmaTransfer transfer_;
        R receiver_;
        DataType * buffer0_;
        DataType * buffer1_;
        F callback_;
    };

    template<class SpiX, dma::DmaLike Dma, class DataType, class F>
    async::Future<void, SpiError> auto writeDoubleBufferedDma(
        SpiX spiX, 
        Dma & dmaDevice, 
        DataType * data[2], 
        std::uint32_t size, 
        F && callback)
    {
        auto transferFactory = dmaDevice.transferDoubleBuffered(
            dma::MemoryAddressPair(reinterpret_cast<std::uint32_t>(data[0]), reinterpret_cast<std::uint32_t>(data[1])),
            dma::PeripheralAddress(spiX.getAddress(board::spi::DR::_Offset{})),
            size);

        using TransferFactoryType = decltype(transferFactory);
        using CallbackType = std::remove_cvref_t<F>;
        
        return async::makeFuture<void, SpiError>(
            [=]<typename R>(R && receiver)
                -> WriteDmaOperation<SpiX, DataType, TransferFactoryType, std::remove_cvref_t<R>, CallbackType>
            {
                return {
                    std::move(transferFactory), 
                    data[0], 
                    data[1]
                };
            });
    }
}