#pragma once
#include "peripheral_types.hpp"
#include "board/interrupts.hpp"
#include "dma.hpp"
#include "board/regmap/dma.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::dma
{
    namespace detail
    {
        template<std::uint8_t deviceId, std::uint8_t streamId> struct DmaStreamId {};

        // Mapping between dma and stream id to interrupt
        constexpr auto getDmaInterrupt(DmaStreamId<0, 0>) { return board::Interrupts::DMA1_Stream0; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 1>) { return board::Interrupts::DMA1_Stream1; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 2>) { return board::Interrupts::DMA1_Stream2; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 3>) { return board::Interrupts::DMA1_Stream3; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 4>) { return board::Interrupts::DMA1_Stream4; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 5>) { return board::Interrupts::DMA1_Stream5; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 6>) { return board::Interrupts::DMA1_Stream6; }
        constexpr auto getDmaInterrupt(DmaStreamId<0, 7>) { return board::Interrupts::DMA1_Stream7; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 0>) { return board::Interrupts::DMA2_Stream0; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 1>) { return board::Interrupts::DMA2_Stream1; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 2>) { return board::Interrupts::DMA2_Stream2; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 3>) { return board::Interrupts::DMA2_Stream3; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 4>) { return board::Interrupts::DMA2_Stream4; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 5>) { return board::Interrupts::DMA2_Stream5; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 6>) { return board::Interrupts::DMA2_Stream6; }
        constexpr auto getDmaInterrupt(DmaStreamId<1, 7>) { return board::Interrupts::DMA2_Stream7; }
    }

    struct DmaId
    {
        constexpr DmaId(std::uint8_t _deviceId, std::uint8_t _streamId)
        : deviceId(_deviceId), streamId(_streamId)
        {

        }

        std::uint8_t deviceId; 
        std::uint8_t streamId;
    };
    
    // DMA channel
    using Channel = board::dma::CR::ChselVal;
    
    // Priority
    using Priority = board::dma::CR::PlVal;

    using DataSize = board::dma::CR::PsizeVal;

    struct DmaConfig
    {
        DmaId id;
        Channel channel;
        DataSize dataSize = DataSize::BYTE;
        Priority priority = Priority::LOW;
    };

    template<DmaConfig config>
    struct makeStream_t 
    {
        template<class Board>
        constexpr auto operator()(Board boardDescriptor) const
        {
            constexpr auto dmaX = boardDescriptor.getPeripheral(PeripheralTypes::DMA<config.id.deviceId>); 
            constexpr auto streamIndex = uint8_c<config.id.streamId>;

            // Setup peripheral
            dmaX.enable();

            // Disable the stream if currently enabled
            if(reg::bitIsSet(dmaX, board::dma::CR::EN[streamIndex]))
            {
                reg::clear(dmaX, board::dma::CR::EN[streamIndex]);
                while(reg::bitIsSet(dmaX, board::dma::CR::EN[streamIndex])) { }
            }

            reg::apply(dmaX, 
                reg::write(board::dma::CR::CHSEL[streamIndex], constant_c<config.channel>),
                reg::write(board::dma::CR::PL[streamIndex], constant_c<config.priority>),
                reg::write(board::dma::CR::PSIZE[streamIndex], constant_c<config.dataSize>),
                reg::set(board::dma::CR::MINC[streamIndex]));

            auto interrupt = detail::getDmaInterrupt(
                detail::DmaStreamId<config.id.deviceId, config.id.streamId>{});
            boardDescriptor.enableIRQ(interrupt);
            auto event = boardDescriptor.getInterruptEvent(interrupt);

            return Dma<decltype(dmaX), config.id.streamId>{event};
        }
    };

    template<DmaConfig config>
    inline constexpr makeStream_t<config> makeStream{};
}