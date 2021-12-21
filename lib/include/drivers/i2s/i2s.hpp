#pragma once
#include "board/regmap/spi.hpp"
#include "drivers/dma/dma_concepts.hpp"
#include "drivers/spi/detail/write_dma.hpp"
#include "async/event.hpp"
#include "async/make_source_sender.hpp"
#include "detail/write.hpp"

namespace drivers::i2s
{
    template<class SpiX>
    class I2S
    {
    public:
        explicit I2S(const async::EventEmitter & interruptSource) : interruptSource_(interruptSource) { }
        I2S(const I2S &) = delete;
        I2S(I2S &&) = default;
        I2S & operator=(const I2S &) = delete;
        I2S & operator=(I2S &&) = delete;

        /**
         * Write data to the peripheral in non-blocking (interrupt) mode.
         * 
         * @param data Array of data to write
         * @param size Array size
         * @return true on success, false otherwise
         */
        auto write(std::uint16_t * data, std::uint32_t size)
        {
            return async::makeSourceSender<spi::SpiError>(
                [this, data, size]<typename R>(R && receiver) -> detail::WriteOperation<SpiX, std::uint16_t, std::remove_cvref_t<R>>
                {
                    return {static_cast<R&&>(receiver), interruptSource_, data, size};
                });
        }

        template<dma::DmaLike Dma>
        auto writeContinuous(Dma & dmaDevice, std::uint16_t * data[2], std::uint32_t size)
        {
            return spi::detail::writeDoubleBufferedDma(SpiX{}, dmaDevice, data, size);
        }

        /*template<class DmaX, class DmaConfig>
        bool writeDma(dma::Dma<DmaX, DmaConfig> & dmaDev, std::uint16_t * data, std::uint32_t size);*/

        /**
         * Start a double buffered dma transmission to the peripheral. The transmission will
         * continue indefinetly.
         * 
         * @param dmaDev Dma device to use. Must be configured in memory to peripheral mode and double buffer mode.
         * @param data Data buffers
         * @param size Data buffer size
         * @param callback Callback that is called whenever one of the buffers becomes available. The 
         * empty buffer is passed along with the buffer size.
         * @return true on success, false otherwise
         */
        /*template<class DmaX, class DmaConfig>
        bool startWriteStream(dma::Dma<DmaX, DmaConfig> & dmaDev, std::uint16_t * data[2], std::uint32_t size, TransferCompleteCallback && callback);*/

    private:
        async::EventEmitter interruptSource_;
    };
}