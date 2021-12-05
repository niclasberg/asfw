#pragma once
#include "types.hpp"
#include "board/regmap/dma.hpp"
#include "dma_modes.hpp"

#include "reg/peripheral_operations.hpp"
#include "reg/write.hpp"
#include "reg/set.hpp"
#include "reg/apply.hpp"
#include "reg/clear.hpp"

namespace drivers::dma::detail
{
    using PlVal = board::dma::CR::PlVal;
    using ChselVal = board::dma::CR::ChselVal;

    template<
        class DmaX,
        std::uint8_t _streamId,
        ChselVal _channel,
        PlVal _priority,
        std::uint32_t _psize,
        DmaMode _mode>
    void initDmaStream(
        DmaX dmaX, 
        uint8_<_streamId> streamIndex,
        constant_<_channel> channel,
        constant_<_priority> priority,
        constant_<_psize> psize,
        constant_<_mode>)
    {
        dmaX.enable();

        // Disable the stream if currently enabled
        if(bitIsSet(dmaX, board::dma::CR::EN[streamIndex]))
        {
            reg::clear(dmaX, board::dma::CR::EN[streamIndex]);
            while(reg::bitIsSet(dmaX, board::dma::CR::EN[streamIndex])) { }
        }

        reg::apply(dmaX, 
            reg::write(board::dma::CR::CHSEL[streamIndex], channel),
            reg::write(board::dma::CR::PL[streamIndex], priority),
            reg::write(board::dma::CR::PSIZE[streamIndex], uint32_c<psize>),
            reg::set(board::dma::CR::MINC[streamIndex]),
            []() 
            {
                if constexpr (_mode == DmaMode::NORMAL)
                {
                    return reg::combine(
                        reg::clear(board::dma::CR::DBM[uint8_c<_streamId>]),
                        reg::clear(board::dma::CR::CIRC[uint8_c<_streamId>]));
                }
                else if constexpr (_mode == DmaMode::CIRCULAR)
                {
                    return reg::combine(
                        reg::clear(board::dma::CR::DBM[uint8_c<_streamId>]),
                        reg::set(board::dma::CR::CIRC[uint8_c<_streamId>]));
                }
                else 
                {
                    return reg::combine(
                        reg::set(board::dma::CR::DBM[uint8_c<_streamId>]),
                        reg::set(board::dma::CR::CIRC[uint8_c<_streamId>]));
                }
            }());
    }
}
