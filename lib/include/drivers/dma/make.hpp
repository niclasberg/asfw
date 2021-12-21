#pragma once
#include "opt/option.hpp"
#include "peripheral_types.hpp"
#include "board/interrupts.hpp"
#include "dma.hpp"
#include "board/regmap/dma.hpp"
#include "reg/bit_is_set.hpp"

namespace drivers::dma
{
    namespace detail
    {
        // Generate unique option tags
        MAKE_OPTION_KEY(CHANNEL);
        MAKE_OPTION_KEY(PRIORITY);
        MAKE_OPTION_KEY(DATA_SIZE);

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

    template<std::uint8_t deviceId, std::uint8_t _streamId>
    constexpr auto streamId = detail::DmaStreamId<deviceId, _streamId>{};
    
    namespace Channel
    {
        constexpr auto CHANNEL0 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL0>);
        constexpr auto CHANNEL1 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL1>);
        constexpr auto CHANNEL2 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL2>);
        constexpr auto CHANNEL3 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL3>);
        constexpr auto CHANNEL4 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL4>);
        constexpr auto CHANNEL5 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL5>);
        constexpr auto CHANNEL6 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL6>);
        constexpr auto CHANNEL7 = opt::makeOption(detail::CHANNEL, constant_c<board::dma::CR::ChselVal::CHANNEL7>);
    }

    namespace Priority
    {
        constexpr auto LOW       = opt::makeOption(detail::PRIORITY, constant_c<board::dma::CR::PlVal::LOW>);
        constexpr auto MEDIUM    = opt::makeOption(detail::PRIORITY, constant_c<board::dma::CR::PlVal::MEDIUM>);
        constexpr auto HIGH      = opt::makeOption(detail::PRIORITY, constant_c<board::dma::CR::PlVal::HIGH>);
        constexpr auto VERY_HIGH = opt::makeOption(detail::PRIORITY, constant_c<board::dma::CR::PlVal::VERY_HIGH>);
    }

    namespace DataSize
    {
        constexpr auto BITS8  = opt::makeOption(detail::DATA_SIZE, uint32_c<0>);
        constexpr auto BITS16 = opt::makeOption(detail::DATA_SIZE, uint32_c<1>);
        constexpr auto BITS32 = opt::makeOption(detail::DATA_SIZE, uint32_c<2>);
    }

    template<class Board, std::uint8_t deviceId, std::uint8_t streamId, class ... Options>
    constexpr auto makeStream(Board boardDescriptor, detail::DmaStreamId<deviceId, streamId> dmaStreamId, Options && ... opts)
    {
        constexpr auto dmaX = boardDescriptor.getPeripheral(PeripheralTypes::DMA<deviceId>); 
        constexpr auto streamIndex = uint8_c<streamId>;

        auto options = opt::makeOptionSet(
            opt::fields(
                detail::CHANNEL, 
                detail::DATA_SIZE, 
                detail::PRIORITY),
            std::forward<Options>(opts)...);

        // Apply default options
        auto channel   = opt::getOrDefault(options, detail::CHANNEL, Channel::CHANNEL0);
        auto priority  = opt::getOrDefault(options, detail::PRIORITY, Priority::LOW);
        auto psize     = opt::getOrDefault(options, detail::DATA_SIZE, DataSize::BITS8);

        // Setup peripheral
        dmaX.enable();

        // Disable the stream if currently enabled
        if(reg::bitIsSet(dmaX, board::dma::CR::EN[streamIndex]))
        {
            reg::clear(dmaX, board::dma::CR::EN[streamIndex]);
            while(reg::bitIsSet(dmaX, board::dma::CR::EN[streamIndex])) { }
        }

        reg::apply(dmaX, 
            reg::write(board::dma::CR::CHSEL[streamIndex], channel),
            reg::write(board::dma::CR::PL[streamIndex], priority),
            reg::write(board::dma::CR::PSIZE[streamIndex], psize),
            reg::set(board::dma::CR::MINC[streamIndex]));

        auto interrupt = detail::getDmaInterrupt(dmaStreamId);
		boardDescriptor.enableIRQ(interrupt);
        auto event = boardDescriptor.getInterruptEvent(interrupt);

        return Dma<decltype(dmaX), streamId>{event};
    }
}