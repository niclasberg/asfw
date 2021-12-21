#pragma once
#include "types.hpp"
#include "detail/dma_modes.hpp"
#include "address.hpp"
#include "async/make_source_sender.hpp"
#include "detail/transfer_operation.hpp"
#include "async/event.hpp"

namespace drivers::dma
{
    template<class DmaX, std::uint8_t streamIndex>
    class Dma
    {
    public:
        explicit Dma(const async::EventEmitter & eventEmitter) : interruptEvent_(eventEmitter) 
        {

        }

        auto transferSingle(MemoryAddress src, MemoryAddress dst, std::uint16_t size)
            -> detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::SINGLE_SHOT, MemoryAddress, MemoryAddress>
        {
            return {interruptEvent_, src, dst, size};
        }

        auto transferSingle(MemoryAddress src, PeripheralAddress dst, std::uint16_t size)
            -> detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::SINGLE_SHOT, MemoryAddress, PeripheralAddress>
        {
            return {interruptEvent_, src, dst, size};
        }

        auto transferSingle(PeripheralAddress src, MemoryAddress dst, std::uint16_t size)
            -> detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::SINGLE_SHOT, PeripheralAddress, MemoryAddress>
        {
            return {interruptEvent_, src, dst, size};
        }

        auto transferDoubleBuffered(MemoryAddressPair src, PeripheralAddress dst, std::uint16_t size)
            -> detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::DOUBLE_BUFFERED, MemoryAddressPair, PeripheralAddress>
        {
            return {interruptEvent_, src, dst, size};
        }

        auto transferDoubleBuffered(PeripheralAddress src, MemoryAddressPair dst, std::uint16_t size)
            -> detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::DOUBLE_BUFFERED, PeripheralAddress, MemoryAddressPair>
        {
            return {interruptEvent_, src, dst, size};
        }
        
    private:
        async::EventEmitter interruptEvent_;
    };
}