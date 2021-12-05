#pragma once
#include "uart.hpp"
#include "opt/option.hpp"
#include "detail/init.hpp"
#include <boost/hana/string.hpp>
#include "drivers/gpio/make.hpp"

namespace drivers::uart
{
    namespace detail
    {
        MAKE_OPTION_KEY(BAUD_RATE);
        MAKE_OPTION_KEY(FRAME_FORMAT);
        MAKE_OPTION_KEY(TX_PIN);
        MAKE_OPTION_KEY(RX_PIN);

        constexpr auto _7E1 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_1, true, PsVal::EVEN>{};
        constexpr auto _7O1 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_1, true, PsVal::ODD>{};
        constexpr auto _8N1 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_1, false>{};
        constexpr auto _8E1 = FrameFormatDef<MVal::DATA_BITS_9, StopVal::STOP_BITS_1, true, PsVal::EVEN>{};
        constexpr auto _8O1 = FrameFormatDef<MVal::DATA_BITS_9, StopVal::STOP_BITS_1, true, PsVal::ODD>{};
        constexpr auto _7E2 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_2, true, PsVal::EVEN>{};
        constexpr auto _7O2 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_2, true, PsVal::ODD>{};
        constexpr auto _8N2 = FrameFormatDef<MVal::DATA_BITS_8, StopVal::STOP_BITS_2, false>{};
        constexpr auto _8E2 = FrameFormatDef<MVal::DATA_BITS_9, StopVal::STOP_BITS_2, true, PsVal::EVEN>{};
        constexpr auto _8O2 = FrameFormatDef<MVal::DATA_BITS_9, StopVal::STOP_BITS_2, true, PsVal::ODD>{};

        template<std::uint8_t deviceId>
        constexpr auto getInterrupt(DeviceId<deviceId>)
        {
            static_assert(deviceId == 3 || deviceId == 4, "Device id out of range");
            if constexpr(deviceId == 3)
            {
                return board::Interrupts::UART4;
            }
            else if constexpr(deviceId == 4)
            {
                return board::Interrupts::UART5;
            }
        }
    }

    template<std::uint32_t value>
    constexpr auto baudRate = opt::makeOption(detail::BAUD_RATE, uint32_c<value>);

    namespace FrameFormat
    {
        constexpr auto _7E1 = opt::makeOption(detail::FRAME_FORMAT, detail::_7E1);
        constexpr auto _7O1 = opt::makeOption(detail::FRAME_FORMAT, detail::_7O1);
        constexpr auto _8N1 = opt::makeOption(detail::FRAME_FORMAT, detail::_8N1);
        constexpr auto _8E1 = opt::makeOption(detail::FRAME_FORMAT, detail::_8E1);
        constexpr auto _8O1 = opt::makeOption(detail::FRAME_FORMAT, detail::_8O1);
        constexpr auto _7E2 = opt::makeOption(detail::FRAME_FORMAT, detail::_7E2);
        constexpr auto _7O2 = opt::makeOption(detail::FRAME_FORMAT, detail::_7O2);
        constexpr auto _8N2 = opt::makeOption(detail::FRAME_FORMAT, detail::_8N2);
        constexpr auto _8E2 = opt::makeOption(detail::FRAME_FORMAT, detail::_8E2);
        constexpr auto _8O2 = opt::makeOption(detail::FRAME_FORMAT, detail::_8O2);
    }

    template<std::uint8_t port, std::uint8_t pin>
    constexpr auto txPin = opt::makeOption(detail::TX_PIN, PIN<port, pin>);

    template<std::uint8_t port, std::uint8_t pin>
    constexpr auto rxPin = opt::makeOption(detail::RX_PIN, PIN<port, pin>);

    template<class Board, std::uint8_t id, class ... Options>
    constexpr auto makeUart(Board board, DeviceId<id> deviceId, Options && ... opts)
    {
        constexpr auto uartX = board.getPeripheral(PeripheralTypes::UART<id>); 

        auto options = opt::makeOptionSet(
            opt::fields(
                detail::BAUD_RATE,
                detail::FRAME_FORMAT,
                detail::TX_PIN,
                detail::RX_PIN),
            std::forward<Options>(opts)...);

        // Setup pins
        auto altFn = hana::if_(
                uint8_c<id> < uint8_c<3>, 
                gpio::AltFn::USART1_3, 
                gpio::AltFn::USART4_6);

        gpio::makeAltFnPin(board, opt::get(options, detail::TX_PIN), altFn, gpio::PuPd::PULL_UP);
        gpio::makeAltFnPin(board, opt::get(options, detail::RX_PIN), altFn);

        // Initialize uart
        detail::initUart(
            uartX, 
            board.getApb1ClockFrequency(),
            opt::get(options, detail::BAUD_RATE), 
            opt::getOrDefault(options, detail::FRAME_FORMAT, detail::_8N1));

        // Setup interrupts
        auto interrupt = detail::getInterrupt(deviceId);
		board.enableIRQ(interrupt);

        return Uart<decltype(uartX)>{
            board.getInterruptEvent(interrupt)};
    }
}