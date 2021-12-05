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
    class SingleShotDma
    {
    public:
        explicit SingleShotDma(const async::EventEmitter & eventEmitter) : interruptEvent_(eventEmitter) 
        {

        }

        auto transfer(MemoryAddress src, MemoryAddress dst, std::uint16_t size)
        {
            return async::makeSourceSender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::NORMAL, MemoryAddress, MemoryAddress>
                    {interruptEvent_, src, dst, size}
            );
        }

        auto transfer(MemoryAddress src, PeripheralAddress dst, std::uint16_t size)
        {
            return async::makeSourceSender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::NORMAL, MemoryAddress, PeripheralAddress>
                    {interruptEvent_, src, dst, size}
            );
        }

        auto transfer(PeripheralAddress src, MemoryAddress dst, std::uint16_t size)
        {
            return async::makeSourceSender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::NORMAL, MemoryAddress, PeripheralAddress>
                    {interruptEvent_, src, dst, size}
            );
        }
        
    private:
        async::EventEmitter interruptEvent_;
    };

    template<class DmaX, std::uint8_t streamId>
    class CircularDma
    {
        /*auto transfer(MemoryAddress src, PeripheralAddress dst, std::uint16_t size)
        {
            return async::makeSourceManySender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::NORMAL, MemoryAddress, MemoryAddress>
                    {interruptEvent_, src, dst, size}
            );
        }

        void transfer(PeripheralAddress src, MemoryAddress dst)
        {
            return async::makeSourceManySender<DmaError>(
                detail::TransferOperationFactory<DmaX, streamIndex, detail::DmaMode::NORMAL, MemoryAddress, MemoryAddress>
                    {interruptEvent_, src, dst, size}
            );
        }*/
    };
}