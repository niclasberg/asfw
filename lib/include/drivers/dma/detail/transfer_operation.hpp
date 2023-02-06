#pragma once
#include "types.hpp"
#include "dma_modes.hpp"
#include "async/event.hpp"
#include "../address.hpp"
#include "../dma_signal.hpp"
#include "board/regmap/dma.hpp"
#include "async/receiver.hpp"

#include "reg/write.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/apply.hpp"
#include "reg/read.hpp"
#include "reg/bit_is_set.hpp"

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
        , handler_(static_cast<R2&&>(receiver))
        {

        }

        DmaTransferOperation(const DmaTransferOperation&) = delete;
        DmaTransferOperation & operator=(const DmaTransferOperation&) = delete;

        bool start()
        {
            if (!interruptEvent_.subscribe(this))
            {
                return false;
            }

            constexpr auto streamIdx = uint8_c<streamIndex>;

            // Clear interrupt flags
            reg::apply(DmaX{}, 
                reg::set(board::dma::IFCR::CTCIF[streamIdx]),
                reg::set(board::dma::IFCR::CTEIF[streamIdx]),
                reg::set(board::dma::IFCR::CDMEIF[streamIdx]),
                reg::set(board::dma::IFCR::CFEIF[streamIdx]),
                reg::set(board::dma::IFCR::CHTIF[streamIdx]));

            // Set address, mode, transfer direction and size
            setMemoryAddressAndDirection(DmaX{}, streamIdx, srcAddress_, dstAddress_);

            if constexpr (mode == DmaMode::SINGLE_SHOT)
            {
                reg::apply(DmaX{},
                    reg::clear(board::dma::CR::DBM[streamIdx]),
                    reg::clear(board::dma::CR::CIRC[streamIdx]));
            }
            else if constexpr (mode == DmaMode::CIRCULAR)
            {
                reg::apply(DmaX{},
                    reg::clear(board::dma::CR::DBM[streamIdx]),
                    reg::set(board::dma::CR::CIRC[streamIdx]));
            }
            else 
            {
                reg::apply(DmaX{},
                    reg::set(board::dma::CR::DBM[streamIdx]),
                    reg::set(board::dma::CR::CIRC[streamIdx]));
            }

            // Enable interrupts
            reg::apply(DmaX{}, 
                reg::set(board::dma::CR::TCIE[streamIdx]),
                reg::set(board::dma::CR::TEIE[streamIdx]),
                reg::clear(board::dma::CR::HTIE[streamIdx]),
                reg::clear(board::dma::CR::DMEIE[streamIdx]));

            reg::write(DmaX{}, board::dma::NDTR::NDT[streamIdx], size_);

            // Enable the dma stream
            reg::set(DmaX{}, board::dma::CR::EN[streamIdx]);

            return true;
        }

        void handleEvent()
        {
            if(reg::bitIsSet(DmaX{}, board::dma::ISR::TEIF[uint8_c<streamIndex>]))
            {
                // Clear transfer error flag
                reg::set(DmaX{}, board::dma::IFCR::CTEIF[uint8_c<streamIndex>]);

                stop();
                handler_(DmaSignal::TRANSFER_ERROR);
                
                return;
            }

            // TODO handle direct mode error, fifo error

            // Transfer half complete
            /*if(reg::bitIsSet(DmaX{}, board::dma::ISR::HTIF[uint8_c<streamIndex>]))
            {
                reg::set(DmaX{}, board::dma::IFCR::CHTIF[uint8_c<streamIndex>]);

                if constexpr (mode == DmaMode::SINGLE_SHOT)
                {
                    handler_(DmaSignal::TRANSFER_HALF_COMPLETE);
                    reg::clear(DmaX{}, board::dma::CR::HTIE);
                }
                else if constexpr (mode == DmaMode::DOUBLE_BUFFERED)
                {
                    if (reg::read(DmaX{}, board::dma::CR::CT[uint8_c<streamIndex>]) == 0)
                    {
                        handler_(DmaSignal::TRANSFER_HALF_COMPLETE_MEMORY1);
                    }
                    else
                    {
                        handler_(DmaSignal::TRANSFER_HALF_COMPLETE);
                    }
                }
                else
                {
                    handler_(DmaSignal::TRANSFER_HALF_COMPLETE);
                }
            }*/

            // Transfer complete
            if (reg::bitIsSet(DmaX{}, board::dma::ISR::TCIF[uint8_c<streamIndex>]))
            {
                reg::set(DmaX{}, board::dma::IFCR::CTCIF[uint8_c<streamIndex>]);

                if constexpr (mode == DmaMode::SINGLE_SHOT)
                {
                    stop();
                    handler_(DmaSignal::TRANSFER_COMPLETE);
                }
                else if constexpr (mode == DmaMode::DOUBLE_BUFFERED)
                {
                    if (reg::read(DmaX{}, board::dma::CR::CT[uint8_c<streamIndex>]) == 0)
                    {
                        handler_(DmaSignal::TRANSFER_COMPLETE_MEMORY1);
                    }
                    else
                    {
                        handler_(DmaSignal::TRANSFER_COMPLETE);
                    }
                }
                else
                {
                    handler_(DmaSignal::TRANSFER_COMPLETE);
                }
            }
        }

        void stop()
        {
            reg::apply(DmaX{}, 
                reg::clear(board::dma::CR::TCIE[uint8_c<streamIndex>]),
                reg::clear(board::dma::CR::TEIE[uint8_c<streamIndex>]));
            
            interruptEvent_.unsubscribe();
        }

    private:
        async::EventEmitter interruptEvent_;
        SrcAddress srcAddress_;
        DstAddress dstAddress_;
        std::uint16_t size_;
        R handler_;
    };

    template<class DmaX, 
        std::uint8_t streamIndex, 
        DmaMode mode,
        class SrcAddress, 
        class DstAddress>
    struct TransferOperationFactory
    {
        template<std::invocable<DmaSignal> R>
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