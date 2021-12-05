#pragma once
#include "types.hpp"
#include "dma_modes.hpp"
#include "async/event.hpp"
#include "../address.hpp"
#include "../dma_error.hpp"
#include "board/regmap/dma.hpp"
#include "async/receiver.hpp"

#include "reg/write.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"

namespace drivers::dma::detail
{
    using DirVal = board::dma::CR::DirVal;

    template<class DmaX, class StreamIndex>
    void setMemoryAddressAndDirection(
        DmaX dmaX, 
        StreamIndex streamIndex, 
        MemoryAddress src,
        PeripheralAddress dst)
    {
        reg::write(dmaX, board::dma::CR::DIR[streamIndex], constant_c<DirVal::MEMORY_TO_PERIPHERAL>);
        reg::write(dmaX, board::dma::PAR::PA[streamIndex], dst.getAddress());
        reg::write(dmaX, board::dma::M0AR::M0A[streamIndex], src.getAddress());
    }

    template<class DmaX, class StreamIndex>
    void setMemoryAddressAndDirection(
        DmaX dmaX, 
        StreamIndex streamIndex, 
        PeripheralAddress src,
        MemoryAddress dst)
    {
        reg::write(dmaX, board::dma::CR::DIR[streamIndex], constant_c<DirVal::PERIPHERAL_TO_MEMORY>);
        reg::write(dmaX, board::dma::PAR::PA[streamIndex], src.getAddress());
        reg::write(dmaX, board::dma::M0AR::M0A[streamIndex], dst.getAddress());
    }

    // Memory-Memory transfer addresses
    template<class DmaX, class StreamIndex>
    void setMemoryAddressAndDirection(
        DmaX dmaX, 
        StreamIndex streamIndex, 
        MemoryAddress src,
        MemoryAddress dst)
    {
        reg::write(dmaX, board::dma::CR::DIR[streamIndex], constant_c<DirVal::MEMORY_TO_MEMORY>);
        reg::write(dmaX, board::dma::PAR::PA[streamIndex], src.getAddress());
        reg::write(dmaX, board::dma::M0AR::M0A[streamIndex], dst.getAddress());
    }

    template<class DmaX, class StreamIndex>
    void setMemoryAddressAndDirection(
        DmaX dmaX, 
        StreamIndex streamIndex, 
        MemoryAddressPair src,
        PeripheralAddress dst)
    {
        reg::write(dmaX, board::dma::CR::DIR[streamIndex], constant_c<DirVal::MEMORY_TO_PERIPHERAL>);
        reg::write(dmaX, board::dma::PAR::PA[streamIndex], dst.getAddress());
        reg::write(dmaX, board::dma::M0AR::M0A[streamIndex], src.getAddress0());
        reg::write(dmaX, board::dma::M1AR::M1A[streamIndex], src.getAddress1());
    }

    template<class DmaX, class StreamIndex>
    void setMemoryAddressAndDirection(
        DmaX dmaX, 
        StreamIndex streamIndex, 
        PeripheralAddress src,
        MemoryAddressPair dst)
    {
        reg::write(dmaX, board::dma::CR::DIR[streamIndex], constant_c<DirVal::PERIPHERAL_TO_MEMORY>);
        reg::write(dmaX, board::dma::PAR::PA[streamIndex], src.getAddress());
        reg::write(dmaX, board::dma::M0AR::M0A[streamIndex], dst.getAddress0());
        reg::write(dmaX, board::dma::M1AR::M1A[streamIndex], dst.getAddress1());
    }

    template<
        class DmaX, 
        std::uint8_t streamIndex, 
        DmaMode mode,
        class SrcAddress, 
        class DstAddress,
        class R>
    class DmaTransferOperation : public async::EventHandlerImpl<
        DmaTransferOperation<DmaX, streamIndex, mode, SrcAddress, DstAddress, R>>
    {
    public:
        template<class R2>
        DmaTransferOperation(
            const async::EventEmitter & interruptEvent,
            SrcAddress srcAddress,
            DstAddress dstAddress,
            std::uint16_t size,
            R2 && receiver)
        : interruptEvent_(interruptEvent)
        , srcAddress_(srcAddress)
        , dstAddress_(dstAddress)
        , size_(size)
        , receiver_(static_cast<R2&&>(receiver))
        {

        }

        void start()
        {
            if (!interruptEvent_.subscribe(this))
            {
                async::setError(std::move(receiver_), DmaError::BUSY);
                return;
            }

            // Clear interrupt flags
            reg::set(DmaX{}, board::dma::IFCR::CTCIF[uint8_c<streamIndex>]);
            reg::set(DmaX{}, board::dma::IFCR::CTEIF[uint8_c<streamIndex>]);

            // Set address, transfer direction and size
            setMemoryAddressAndDirection(DmaX{}, uint8_c<streamIndex>, srcAddress_, dstAddress_);
            reg::write(DmaX{}, board::dma::NDTR::NDT[uint8_c<streamIndex>], size_);

            // Enable interrupts
            reg::set(DmaX{}, board::dma::CR::TCIE[uint8_c<streamIndex>]);
            reg::set(DmaX{}, board::dma::CR::TEIE[uint8_c<streamIndex>]);

            // Enable the dma stream
            reg::set(DmaX{}, board::dma::CR::EN[uint8_c<streamIndex>]);
        }

        void handleEvent()
        {
            if(reg::bitIsSet(DmaX{}, board::dma::ISR::TEIF[uint8_c<streamIndex>]))
            {
                // Clear transfer error flag
                reg::set(DmaX{}, board::dma::IFCR::CTEIF[uint8_c<streamIndex>]);

                async::setError(std::move(receiver_), DmaError::TRANSFER_ERROR);
            }

            // TODO handle direct mode error, fifo error

            // Transfer complete
            if (reg::bitIsSet(DmaX{}, board::dma::ISR::TCIF[uint8_c<streamIndex>]))
            {
                // Clear interrupt flag
                reg::set(DmaX{}, board::dma::IFCR::CTCIF[uint8_c<streamIndex>]);

                if constexpr (mode == DmaMode::NORMAL)
                {
                    stop();
                    async::setValue(std::move(receiver_));
                }
                else
                {
                    // In circular and double buffer mode, we don't want to 
                    // stop the DMA transfer at this point. Just send the next
                    // value
                    async::setNext(receiver_);
                }
            }
        }

        void stop()
        {
            reg::clear(DmaX{}, board::dma::CR::TCIE[uint8_c<streamIndex>]);
            reg::clear(DmaX{}, board::dma::CR::TEIE[uint8_c<streamIndex>]);
        }

    private:
        async::EventEmitter interruptEvent_;
        SrcAddress srcAddress_;
        DstAddress dstAddress_;
        std::uint16_t size_;
        R receiver_;
    };

    template<class DmaX, 
        std::uint8_t streamIndex, 
        DmaMode mode,
        class SrcAddress, 
        class DstAddress>
    struct TransferOperationFactory
    {
        template<class R>
        auto operator()(R && receiver) const
            -> DmaTransferOperation<DmaX, streamIndex, mode, SrcAddress, DstAddress, std::remove_cvref_t<R>>
        {
            return {interruptEvent_, srcAddress_, dstAddress_, size_, static_cast<R&&>(receiver)};
        }

        async::EventEmitter interruptEvent_;
        SrcAddress srcAddress_;
        DstAddress dstAddress_;
        std::uint16_t size_;
    };
}