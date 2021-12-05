#pragma once
#include "detail/init.hpp"
#include "opt/option.hpp"
#include "peripheral_types.hpp"
#include "board/interrupts.hpp"
#include "single_shot_dma.hpp"
#include "double_buffered_dma.hpp"
#include "board/regmap/dma.hpp"

namespace drivers::dma
{
    namespace detail
    {
        // Generate unique option tags
        MAKE_OPTION_KEY(MODE);
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
        constexpr auto CHANNEL0 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL0>);
        constexpr auto CHANNEL1 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL1>);
        constexpr auto CHANNEL2 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL2>);
        constexpr auto CHANNEL3 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL3>);
        constexpr auto CHANNEL4 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL4>);
        constexpr auto CHANNEL5 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL5>);
        constexpr auto CHANNEL6 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL6>);
        constexpr auto CHANNEL7 = opt::makeOption(detail::CHANNEL, constant_c<detail::ChselVal::CHANNEL7>);
    }

    namespace Priority
    {
        constexpr auto LOW       = opt::makeOption(detail::PRIORITY, constant_c<detail::PlVal::LOW>);
        constexpr auto MEDIUM    = opt::makeOption(detail::PRIORITY, constant_c<detail::PlVal::MEDIUM>);
        constexpr auto HIGH      = opt::makeOption(detail::PRIORITY, constant_c<detail::PlVal::HIGH>);
        constexpr auto VERY_HIGH = opt::makeOption(detail::PRIORITY, constant_c<detail::PlVal::VERY_HIGH>);
    }

    namespace DataSize
    {
        constexpr auto BITS8  = opt::makeOption(detail::DATA_SIZE, uint32_c<0>);
        constexpr auto BITS16 = opt::makeOption(detail::DATA_SIZE, uint32_c<1>);
        constexpr auto BITS32 = opt::makeOption(detail::DATA_SIZE, uint32_c<2>);
    }

    namespace Mode
    {
        constexpr auto NORMAL        = opt::makeOption(detail::MODE, constant_c<detail::DmaMode::NORMAL>);
        constexpr auto CIRCULAR      = opt::makeOption(detail::MODE, constant_c<detail::DmaMode::CIRCULAR>);
        constexpr auto DOUBLE_BUFFER = opt::makeOption(detail::MODE, constant_c<detail::DmaMode::DOUBLE_BUFFERED>);
    }

    template<class Board, std::uint8_t deviceId, std::uint8_t streamId, class ... Options>
    constexpr auto makeStream(Board boardDescriptor, detail::DmaStreamId<deviceId, streamId> dmaStreamId, Options && ... opts)
    {
        constexpr auto dmaX = boardDescriptor.getPeripheral(PeripheralTypes::DMA<deviceId>); 

        auto options = opt::makeOptionSet(
            opt::fields(
                detail::CHANNEL, 
                detail::DATA_SIZE, 
                detail::MODE, 
                detail::PRIORITY),
            std::forward<Options>(opts)...);

        // Apply default options
        auto channel   = opt::getOrDefault(options, detail::CHANNEL, Channel::CHANNEL0);
        auto priority  = opt::getOrDefault(options, detail::PRIORITY, Priority::LOW);
        auto mode      = opt::getOrDefault(options, detail::MODE, Mode::NORMAL);
        auto psize     = opt::getOrDefault(options, detail::DATA_SIZE, DataSize::BITS8);

        detail::initDmaStream(
            dmaX, 
            uint8_c<streamId>, 
            channel, 
            priority, 
            psize, 
            mode);

        auto interrupt = detail::getDmaInterrupt(dmaStreamId);
		boardDescriptor.enableIRQ(interrupt);
        auto event = boardDescriptor.getInterruptEvent(interrupt);

        if constexpr (hana::value(mode) == detail::DmaMode::NORMAL)
        {
            return SingleShotDma<decltype(dmaX), streamId>{event};
        }
        else if constexpr (hana::value(mode) == detail::DmaMode::DOUBLE_BUFFERED)
        {
            return DoubleBufferedDma<decltype(dmaX), streamId>{event};
        }
    }
}