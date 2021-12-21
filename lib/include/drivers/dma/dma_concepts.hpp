#pragma once
#include "address.hpp"
#include "dma_signal.hpp"
#include <concepts>

namespace drivers::dma
{   
    namespace detail
    {
        struct DmaHandlerArchetype
        {
            void operator()(DmaSignal);
        };
    }

    template<class T>
    concept DmaTransfer = requires(T & op)
        {
            { op.start() } -> std::same_as<bool>;
            { op.stop() };
        };

    template<class T>
    concept DmaTransferFactory = requires(T && transferFactory, detail::DmaHandlerArchetype & handler)
        {
            { transferFactory(handler) } -> DmaTransfer;
        };

    template<class T>
    concept DmaLike = requires(
            T & dma, 
            MemoryAddressPair memoryAddressPair, 
            PeripheralAddress peripheralAddress, 
            std::uint16_t size)
        {
            { dma.transferDoubleBuffered(memoryAddressPair, peripheralAddress, size) }  -> DmaTransferFactory;
            { dma.transferDoubleBuffered(peripheralAddress, memoryAddressPair, size) } -> DmaTransferFactory;
        };

    template<DmaTransferFactory TransferFactory, std::invocable<DmaSignal> Handler>
    using DmaTransferType = decltype(std::declval<TransferFactory&&>()(std::declval<Handler&&>()));
}