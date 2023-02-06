#pragma once
#include "uart.hpp"
#include "drivers/gpio/make.hpp"

namespace drivers::uart
{
    // Frame format for the uart packets
    enum class FrameFormat
    {
        // 7 data bits, 1 stop bit, even parity check
        _7E1,
        // 7 data bits, 1 stop bit, odd parity check
        _7O1,
        // 8 data bits, 1 stop bit, no parity check
        _8N1,
        // 8 data bits, 1 stop bit, even parity check
        _8E1,
        // 8 data bits, 1 stop bit, odd parity check
        _8O1,
        // 7 data bits, 2 stop bits, even parity check
        _7E2,
        // 7 data bits, 2 stop bits, odd parity check
        _7O2,
        // 8 data bits, 2 stop bits, no parity check
        _8N2,
        // 8 data bits, 2 stop bits, even parity check
        _8E2,
        // 8 data bits, 2 stop bits, odd parity check
        _8O2
    };

    struct UartId
    {
        constexpr UartId(std::uint8_t id): value(id) {}

        std::uint8_t value;
    };

    struct UartConfig
    {
        UartId id;
        std::uint32_t baudRate;
        Pin txPin;
        Pin rxPin;
        FrameFormat frameFormat = FrameFormat::_8N1;
    };

    namespace detail
    {
        using board::uart::CR1::PsVal;
        using board::uart::CR1::MVal;
        using board::uart::CR2::StopVal;

        template<class UartX, MVal _dataBits, StopVal _stopBits, bool _parityOn, PsVal _parity>
        void configureFrameFormatImpl()
        {
            reg::write(UartX{}, board::uart::CR2::STOP, constant_c<_stopBits>);

            reg::apply(UartX{}, 
                reg::write(board::uart::CR1::PCE, bool_c<_parityOn>),
                reg::write(board::uart::CR1::PS, constant_c<_parity>),
                reg::write(board::uart::CR1::M, constant_c<_dataBits>),
                // Is this call needed? 
                reg::set (board::uart::CR1::TE));
        }

        template<class UartX, FrameFormat frameFormat>
        void configureFrameFormat()
        {
            if constexpr (frameFormat == FrameFormat::_7E1) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_1, true, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_7O1) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_1, true, PsVal::ODD>();
            else if constexpr (frameFormat == FrameFormat::_8N1) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_1, false, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_8E1) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_9, StopVal::STOP_BITS_1, true, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_8O1) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_9, StopVal::STOP_BITS_1, true, PsVal::ODD>();
            else if constexpr (frameFormat == FrameFormat::_7E2) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_2, true, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_7O2) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_2, true, PsVal::ODD>();
            else if constexpr (frameFormat == FrameFormat::_8N2) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_8, StopVal::STOP_BITS_2, false, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_8E2) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_9, StopVal::STOP_BITS_2, true, PsVal::EVEN>();
            else if constexpr (frameFormat == FrameFormat::_8O2) 
                configureFrameFormatImpl<UartX, MVal::DATA_BITS_9, StopVal::STOP_BITS_2, true, PsVal::ODD>();
        }

        template<std::uint8_t deviceId>
        constexpr auto getInterrupt()
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

    template<UartConfig config>
    struct makeUart_t
    {
        template<class Board>
        constexpr auto operator()(Board board) const
            -> Uart<decltype(std::declval<Board>().getPeripheral(PeripheralTypes::UART<config.id.value>))>
        {
            auto uartX = board.getPeripheral(PeripheralTypes::UART<config.id.value>); 
            constexpr std::uint32_t peripheralClockFreq = round(board.getApb1ClockFrequency());

            // Setup pins
            gpio::makeAltFnPin<gpio::AltFnPinConfig {
                .pin = config.txPin,
                .altFn = config.id.value < 3 ? gpio::AltFn::USART1_3 : gpio::AltFn::USART4_6,
                .pullUpDown = gpio::PuPd::PULL_UP
            }>(board);
            gpio::makeAltFnPin<gpio::AltFnPinConfig {
                .pin = config.txPin,
                .altFn = config.id.value < 3 ? gpio::AltFn::USART1_3 : gpio::AltFn::USART4_6
            }>(board);

            uartX.enable();
            reg::set(uartX, board::uart::CR1::UE);

            // Configure baud rate generator
            // baud rate = pClockFrequency / (16 * usartDivider)
            // where usartDivider = div_Mantissa.(div_fraction/16)
            constexpr auto dividerTimes16 = 
                (peripheralClockFreq + (config.baudRate/2)) / config.baudRate;
            constexpr auto mantissa = dividerTimes16 / 16;
            constexpr auto fraction = (dividerTimes16 - 16 * mantissa);

            reg::apply(uartX, 
                reg::write(board::uart::BRR::DIV_Mantissa, constant_c<mantissa>),
                reg::write(board::uart::BRR::DIV_Fraction, constant_c<fraction>));

            detail::configureFrameFormat<decltype(uartX), config.frameFormat>();

            // Setup interrupts
            auto interrupt = detail::getInterrupt<config.id.value>();
            board.enableIRQ(interrupt);

            return Uart<decltype(uartX)>{board.getInterruptEvent(interrupt)};
        }   
    };

    template<UartConfig config>
    inline constexpr makeUart_t<config> makeUart {};
}