#pragma once
#include "detail/init.hpp"
#include "peripheral_types.hpp"
#include "spi.hpp"
#include "detail/interrupt.hpp"
#include <boost/hana/string.hpp>
#include "opt/option.hpp"

namespace drivers::spi
{
    namespace detail
    {
        MAKE_OPTION_KEY(IS_MASTER);
        MAKE_OPTION_KEY(BUS_CONFIG);
        MAKE_OPTION_KEY(CLK_POL);
        MAKE_OPTION_KEY(DFF);
        MAKE_OPTION_KEY(CLK_PHASE);
        MAKE_OPTION_KEY(LSB_FIRST);
        MAKE_OPTION_KEY(BR_DIVIDER);
        MAKE_OPTION_KEY(MOSI_PIN);
        MAKE_OPTION_KEY(MISO_PIN);
    }

    /**
     * Data pin: input for master, output for slave
     */
    template<std::uint8_t port, std::uint8_t pin>
    constexpr auto MISO_PIN = opt::makeOption(detail::MISO_PIN, PIN<port, pin>);

    /**
     * Data pin: output for master, input for slave
     */
    template<std::uint8_t port, std::uint8_t pin>
    constexpr auto MOSI_PIN = opt::makeOption(detail::MOSI_PIN, PIN<port, pin>);

    namespace DeviceMode
    {
        constexpr auto MASTER = opt::makeOption(detail::IS_MASTER, true_c);
        constexpr auto SLAVE  = opt::makeOption(detail::IS_MASTER, false_c);
    }

    namespace BitOrder
    {
        constexpr auto LSB_FIRST = opt::makeOption(detail::LSB_FIRST, true_c);
        constexpr auto MSB_FIRST = opt::makeOption(detail::LSB_FIRST, false_c);
    }

    namespace FrameFormat
    {
        constexpr auto BYTE      = opt::makeOption(detail::DFF, constant_c<detail::DffVal::BYTE>);
        constexpr auto HALF_WORD = opt::makeOption(detail::DFF, constant_c<detail::DffVal::HALF_WORD>);
    }

    namespace BusConfig
    {
        constexpr auto FULL_DUPLEX = opt::makeOption(detail::BUS_CONFIG, constant_c<detail::BusConfig::FullDuplex>);
        constexpr auto HALF_DUPLEX = opt::makeOption(detail::BUS_CONFIG, constant_c<detail::BusConfig::HalfDuplex>);
        constexpr auto READ_ONLY   = opt::makeOption(detail::BUS_CONFIG, constant_c<detail::BusConfig::RxOnly>);
        constexpr auto WRITE_ONLY  = opt::makeOption(detail::BUS_CONFIG, constant_c<detail::BusConfig::RxOnly>);
    }

    namespace ClockPolarity 
    {
        constexpr auto LOW  = opt::makeOption(detail::CLK_POL, constant_c<detail::CpolVal::LOW>);
        constexpr auto HIGH = opt::makeOption(detail::CLK_POL, constant_c<detail::CpolVal::HIGH>);
    }

    namespace ClockPhase
    {
        constexpr auto LOW  = opt::makeOption(detail::CLK_PHASE, constant_c<detail::CphaVal::LOW>);
        constexpr auto HIGH = opt::makeOption(detail::CLK_PHASE, constant_c<detail::CphaVal::HIGH>);
    }

    namespace BaudRateDivider
    {
        constexpr auto DIV_2   = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV2>);
        constexpr auto DIV_4   = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV4>);
        constexpr auto DIV_8   = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV8>);
        constexpr auto DIV_16  = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV16>);
        constexpr auto DIV_32  = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV32>);
        constexpr auto DIV_64  = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV64>);
        constexpr auto DIV_128 = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV128>);
        constexpr auto DIV_256 = opt::makeOption(detail::BR_DIVIDER, constant_c<detail::BrVal::PCKL_DIV256>);
    }

    template<class Board, std::uint8_t id, class ... Options>
    auto makeSpi(Board board, DeviceId<id> deviceId, Options && ... opts)
    {
        constexpr auto spiX = board.getPeripheral(PeripheralTypes::SPI<id>); 

        auto options = opt::makeOptionSet(
            opt::fields(
                detail::CLK_POL, 
                detail::CLK_PHASE, 
                detail::LSB_FIRST,
                detail::BR_DIVIDER,
                detail::DFF,
                detail::BUS_CONFIG,
                detail::IS_MASTER,
                detail::MISO_PIN,
                detail::MOSI_PIN),
            std::forward<Options>(opts)...);

        auto dataFrameFormat = opt::getOrDefault(options, detail::DFF, FrameFormat::BYTE);
        auto busConfig = opt::getOrDefault(options, detail::BUS_CONFIG, BusConfig::FULL_DUPLEX);

        detail::initSpi(
            spiX, 
            opt::getOrDefault(options, detail::CLK_POL, ClockPolarity::LOW), 
            opt::getOrDefault(options, detail::CLK_PHASE, ClockPhase::LOW),
            opt::getOrDefault(options, detail::BR_DIVIDER, BaudRateDivider::DIV_2),
            dataFrameFormat, 
            busConfig,
            opt::getOrDefault(options, detail::IS_MASTER, DeviceMode::MASTER),
            opt::getOrDefault(options, detail::LSB_FIRST, false_c));

        using DataType = std::conditional_t<
            hana::value(dataFrameFormat) == detail::DffVal::BYTE,
            std::uint8_t,
            std::uint16_t>;
        using SpiX = decltype(spiX);

        auto interrupt = detail::getInterrupt(deviceId);
		board.enableIRQ(interrupt);

        return Spi<SpiX, DataType, hana::value(busConfig)>{
            board.getInterruptEvent(interrupt) };
    }
}