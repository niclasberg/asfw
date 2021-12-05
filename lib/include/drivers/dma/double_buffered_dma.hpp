#pragma once
#include "detail/dma_modes.hpp"
#include "address.hpp"
#include "async/make_source_sender.hpp"
#include "detail/transfer_operation.hpp"
#include "async/event.hpp"

namespace drivers::dma
{
    template<class DmaX, std::uint8_t streamIndex>
    class DoubleBufferedDma
    {
    public:
        explicit DoubleBufferedDma(const async::EventEmitter & eventEmitter) : interruptEvent_(eventEmitter) 
        {

        }

        auto transfer(MemoryAddressPair src, PeripheralAddress dst, std::uint16_t size)
        {
            return async::makeSourceManySender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::DOUBLE_BUFFERED, MemoryAddressPair, PeripheralAddress>
                    {interruptEvent_, src, dst, size}
            );
        }

        auto transfer(PeripheralAddress src, MemoryAddressPair dst, std::uint16_t size)
        {
            return async::makeSourceManySender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::DOUBLE_BUFFERED, PeripheralAddress, MemoryAddressPair>
                    {interruptEvent_, src, dst, size}
            );
        }
        
    private:
        async::EventEmitter interruptEvent_;
    };
}