#pragma once
#include "detail/init.hpp"
#include "i2s.hpp"
#include <utility>
#include "board/clock/config.hpp"
#include "drivers/spi/detail/interrupt.hpp"
#include "drivers/gpio/make.hpp"
#include "opt/option.hpp"
#include "peripheral_types.hpp"

namespace drivers::i2s
{
    namespace detail
    {
        // Generate unique option tags
        MAKE_OPTION_KEY(MODE);
        MAKE_OPTION_KEY(STANDARD);
        MAKE_OPTION_KEY(BIT_DEPTH);
        MAKE_OPTION_KEY(FRAME_FORMAT);
        MAKE_OPTION_KEY(MCKL_OUT);
        MAKE_OPTION_KEY(CLK_POL);
        MAKE_OPTION_KEY(SAMPLE_FREQ);
        MAKE_OPTION_KEY(TRANSFER_MODE);
        MAKE_OPTION_KEY(MASTER);
        MAKE_OPTION_KEY(IO_POLICY);
        MAKE_OPTION_KEY(SD_PIN);
        MAKE_OPTION_KEY(WS_PIN);
        MAKE_OPTION_KEY(SCLK_PIN);
        MAKE_OPTION_KEY(MCLK_PIN);

        struct tx_only_tag {};
        struct rx_only_tag {};
    }

    template<std::uint32_t value>
    constexpr auto sampleFreq = opt::makeOption(detail::SAMPLE_FREQ, uint32_c<value>);

    namespace Standard
    {
        constexpr auto PHILIPS = opt::makeOption(detail::STANDARD, constant_c<detail::I2sStdVal::PHILIPS>);
        constexpr auto MSB     = opt::makeOption(detail::STANDARD, constant_c<detail::I2sStdVal::MSB>);
        constexpr auto LSB     = opt::makeOption(detail::STANDARD, constant_c<detail::I2sStdVal::LSB>);
        constexpr auto PCM     = opt::makeOption(detail::STANDARD, constant_c<detail::I2sStdVal::PCM>);
    }

    namespace BitDepth
    {
        constexpr auto BITS16 = opt::makeOption(detail::BIT_DEPTH, false_c);
        constexpr auto BITS32 = opt::makeOption(detail::BIT_DEPTH, true_c);
    }

    namespace TransferMode
    {
        constexpr auto TX_ONLY = opt::makeOption(detail::TRANSFER_MODE, detail::tx_only_tag{});
        constexpr auto RX_ONLY = opt::makeOption(detail::TRANSFER_MODE, detail::rx_only_tag{});
    }

    namespace DataFrameFormat {
        constexpr auto BITS16 = opt::makeOption(detail::FRAME_FORMAT, constant_c<detail::DataLenVal::BITS16>); 
        constexpr auto BITS24 = opt::makeOption(detail::FRAME_FORMAT, constant_c<detail::DataLenVal::BITS24>); 
        constexpr auto BITS32 = opt::makeOption(detail::FRAME_FORMAT, constant_c<detail::DataLenVal::BITS32>); 
    }

    namespace ClkPolarity 
    {
        constexpr auto LOW  = opt::makeOption(detail::CLK_POL, constant_c<detail::CkPolVal::LOW>);
        constexpr auto HIGH = opt::makeOption(detail::CLK_POL, constant_c<detail::CkPolVal::HIGH>);
    }

    namespace MasterClockOut 
    {
        constexpr auto OFF = opt::makeOption(detail::MCKL_OUT, false_c);
        constexpr auto ON  = opt::makeOption(detail::MCKL_OUT, true_c);
    } 

	template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto serialDataPin = opt::makeOption(detail::SD_PIN, PIN<port, pin>);

    template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto serialClockPin = opt::makeOption(detail::SCLK_PIN, PIN<port, pin>);

    template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto masterClockPin = opt::makeOption(detail::MCLK_PIN, PIN<port, pin>);

    template<std::uint8_t port, std::uint8_t pin> 
	constexpr auto wordSelectPin = opt::makeOption(detail::WS_PIN, PIN<port, pin>);

    namespace detail
    {
        struct slave_tag {};
        struct master_tag {};

        constexpr auto getI2sMode(slave_tag, tx_only_tag) { return integral_c<I2sCfgVal, I2sCfgVal::SLAVE_TX>; }
        constexpr auto getI2sMode(slave_tag, rx_only_tag) { return integral_c<I2sCfgVal, I2sCfgVal::SLAVE_RX>; }
        constexpr auto getI2sMode(master_tag, tx_only_tag) { return integral_c<I2sCfgVal, I2sCfgVal::MASTER_TX>; }
        constexpr auto getI2sMode(master_tag, rx_only_tag) { return integral_c<I2sCfgVal, I2sCfgVal::MASTER_RX>; }
    }

    template<class Board, std::uint8_t id, class ... Options>
    auto makeI2sMaster(Board boardDescriptor, DeviceId<id> deviceId, Options && ... opts)
    {
        constexpr auto spiX = boardDescriptor.getPeripheral(PeripheralTypes::SPI<id>); 

        auto options = opt::makeOptionSet(
            opt::fields(
                detail::BIT_DEPTH,
                detail::CLK_POL,
                detail::FRAME_FORMAT,
                detail::IO_POLICY,
                detail::MCKL_OUT,
                detail::SAMPLE_FREQ,
                detail::STANDARD,
                detail::TRANSFER_MODE,
                detail::SD_PIN,
                detail::WS_PIN,
                detail::SCLK_PIN,
                detail::MCLK_PIN),
            std::forward<Options>(opts)...);

        // Apply default options
        auto sampleFrequency = opt::get(options, detail::SAMPLE_FREQ);
        auto mcklOut = opt::getOrDefault(options, detail::MCKL_OUT, false_c);
        auto clockPolarity = opt::getOrDefault(options, detail::CLK_POL, ClkPolarity::LOW);
        auto i2sStandard = opt::getOrDefault(options, detail::STANDARD, Standard::PHILIPS);
        auto transferMode = opt::getOrDefault(options, detail::TRANSFER_MODE, TransferMode::TX_ONLY);
        auto i2sMode = getI2sMode(detail::master_tag{}, transferMode);
        auto bitDepth = opt::getOrDefault(options, detail::BIT_DEPTH, BitDepth::BITS16);
        auto frameFormat = opt::getOrDefault(options, detail::FRAME_FORMAT, DataFrameFormat::BITS16);

        // Configure pins
        gpio::makeAltFnPin(
			boardDescriptor,
			opt::get(options, detail::SCLK_PIN), 
			gpio::AltFn::SPI3);
        gpio::makeAltFnPin(
			boardDescriptor,
			opt::get(options, detail::WS_PIN), 
			gpio::AltFn::SPI3);
        gpio::makeAltFnPin(
			boardDescriptor,
			opt::get(options, detail::SD_PIN), 
			gpio::AltFn::SPI3);
        
        if constexpr (mcklOut)
        {
            gpio::makeAltFnPin(
			    boardDescriptor,
                opt::get(options, detail::MCLK_PIN), 
                gpio::AltFn::SPI3);
        }

        // Configure peripheral
        spiX.enable();
        detail::configureI2sClock(spiX, boardDescriptor.getI2SClockFrequency(), sampleFrequency, bitDepth, mcklOut);
        detail::initI2S(spiX, clockPolarity, i2sStandard, i2sMode, bitDepth, frameFormat);

        // Enable interrupt 
        auto interrupt = spi::detail::getInterrupt(deviceId);
		boardDescriptor.enableIRQ(interrupt);

        return I2S<decltype(spiX)>{
            boardDescriptor.getInterruptEvent(interrupt) };
    }  
}
