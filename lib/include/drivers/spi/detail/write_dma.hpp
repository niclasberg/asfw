#pragma once
#include "async/receiver.hpp"
#include "async/sender.hpp"
#include "async/scheduler.hpp"
#include "drivers/dma/dma_concepts.hpp"
#include "drivers/dma/dma_signal.hpp"
#include "../spi_error.hpp"
#include <array>

#include "delegate.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::spi::detail
{
    template<class SpiX, class DataType, class TransferFactory, class R>
    class WriteDmaOperation
    {
        struct DmaEventHandler
        {
            void operator()(dma::DmaSignal signal)
            {
                switch (signal)
                {
                    case dma::DmaSignal::TRANSFER_COMPLETE:
                        op_.setBuffer0Next();
                        break;
                    case dma::DmaSignal::TRANSFER_COMPLETE_MEMORY1:
                        op_.setBuffer1Next();
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

    private:
        void setBuffer0Next()
        {
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&WriteDmaOperation::setBuffer0NextImpl>, *this});
        }

        void setBuffer1Next()
        {
            auto & s = async::getScheduler(receiver_);
            s.postFromISR({memFn<&WriteDmaOperation::setBuffer1NextImpl>, *this});
        }

        void setBuffer0NextImpl()
        {
            async::setNext(receiver_, buffer0_);
        }

        void setBuffer1NextImpl()
        {
            async::setNext(receiver_, buffer1_);
        }

        void stop()
        {
            reg::clear(SpiX{}, board::spi::CR2::TXDMAEN);
        }

        DmaTransfer transfer_;
        R receiver_;
        DataType * buffer0_;
        DataType * buffer1_;
    };

    template<class SpiX, class DataType, dma::DmaTransferFactory TransferFactory>
    struct WriteDmaSender
    {
        template<template<typename...> class Variant, template<typename...> class Tuple>
        using value_types = Variant<Tuple<DataType *>>;

        template<template<typename...> class Variant>
        using error_types = Variant<SpiError>;

        template<class R>
        auto connect(R && receiver) && 
            -> WriteDmaOperation<SpiX, DataType, TransferFactory, std::remove_cvref_t<R>>
        {
            return {std::move(_transferFactory), static_cast<R&&>(receiver), buffer0_, buffer1_};
        }

        TransferFactory _transferFactory;
        DataType * buffer0_;
        DataType * buffer1_;
    };

    template<class SpiX, dma::DmaLike Dma, class DataType>
    auto writeDoubleBufferedDma(SpiX spiX, Dma & dmaDevice, DataType * data[2], std::uint32_t size)
    {
        auto transferFactory = dmaDevice.transferDoubleBuffered(
            dma::MemoryAddressPair(reinterpret_cast<std::uint32_t>(data[0]), reinterpret_cast<std::uint32_t>(data[1])),
            dma::PeripheralAddress(spiX.getAddress(board::spi::DR::_Offset{})),
            size);
        
        return WriteDmaSender<SpiX, DataType, decltype(transferFactory)>{
            std::move(transferFactory), 
            data[0], 
            data[1]
        };
    }

    /*template<class SpiX, class DataType, dma::DmaTransferFactory TransferFactory>
    auto writeDma(SpiX, TransferFactory && transferFactory, DataType * data[2])
    {
        return async::makeSourceManySender<spi::SpiError, std::uint16_t *>(
            [transferFactory = static_cast<TransferFactory&&>(transferFactory), data]<typename R>(R && receiver) mutable
                -> WriteDmaOperation<SpiX, DataType, std::remove_cvref_t<TransferFactory>, std::remove_cvref_t<R>>
            {
                return {std::move(transferFactory), static_cast<R&&>(receiver), data};
            });
    }*/
}