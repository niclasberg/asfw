#pragma once
#include "address.hpp"
#include "dma_signal.hpp"
#include <concepts>

namespace drivers::dma
{
    template<class T>
    concept DmaHandler = requires(T & handler)
        {
            handler.handleDmaEvent()
        };
    
    namespace detail
    {
        struct DmaHandlerArchetype
        {
            void handleDmaEvent(DmaSignal);
        };
    }

    template<class T>
    concept DmaTransfer = requires(T & op)
        {
            { op.start() } -> std::same_as<bool>;
            { op.stop() } -> std::same_as<bool>;
        };

    template<class T>
    concept DoubleBufferedDmaLike = requires(
            T & dma, 
            MemoryAddressPair memoryAddressPair, 
            PeripheralAddress peripheralAddress, 
            std::uint16_t size,
            DmaHandlerArchetype & handler)
        {
            { dma.transfer(memoryAddressPair, peripheralAddress, size, handler) } -> DmaTransfer;
            { dma.transfer(peripheralAddress, memoryAddressPair, size, handler) } -> DmaTransfer;
        };
}