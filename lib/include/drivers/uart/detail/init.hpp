#pragma once
#include "types.hpp"
#include "reg/peripheral_operations.hpp"
#include "board/regmap/uart.hpp"
#include "reg/set.hpp"
#include "reg/write.hpp"
#include "reg/apply.hpp"

namespace drivers::uart
{
    namespace detail
    {
        using board::uart::CR1::PsVal;
        using board::uart::CR1::MVal;
        using board::uart::CR2::StopVal;

        template<MVal _dataBits, StopVal _stopBits, bool _parityOn, PsVal _parity = PsVal::EVEN>
        struct FrameFormatDef
        { 
            constexpr bool_<_parityOn> getParityEnabled() const { return {}; }
            constexpr integral_constant<PsVal, _parity> getParity() const { return {}; }
            constexpr integral_constant<MVal, _dataBits> getDataBits() const { return {}; }
            constexpr integral_constant<StopVal, _stopBits> getStopBits() const { return {}; }
        };

        template<
            class UartX,
            std::uint32_t _peripheralClockFrequency, 
            std::uint32_t _baudRate,
            MVal _dataBits, 
            StopVal _stopBits, 
            bool _parityOn, 
            PsVal _parity>
        void initUart(
            UartX uartX, 
            constant_<_peripheralClockFrequency> peripheralClockFrequency,
            constant_<_baudRate> baudRate,
            FrameFormatDef<_dataBits, _stopBits, _parityOn, _parity>)
        {
            uartX.enable();
            reg::set(uartX, board::uart::CR1::UE);

            // Configure baud rate generator
            // baud rate = pClockFrequency / (16 * usartDivider)
            // where usartDivider = div_Mantissa.(div_fraction/16)
            auto dividerTimes16 = 
                (peripheralClockFrequency + uint32_c<_baudRate/2>) / baudRate;
            auto mantissa = dividerTimes16 / uint32_c<16>;
            auto fraction = (dividerTimes16 - uint32_c<16> * mantissa);

            reg::apply(uartX, 
                reg::write(board::uart::BRR::DIV_Mantissa, mantissa),
                reg::write(board::uart::BRR::DIV_Fraction, fraction));

            reg::write(uartX, board::uart::CR2::STOP, constant_c<_stopBits>);

            reg::apply(uartX, 
                reg::write(board::uart::CR1::PCE, bool_c<_parityOn>),
                reg::write(board::uart::CR1::PS, constant_c<_parity>),
                reg::write(board::uart::CR1::M, constant_c<_dataBits>),
                reg::set (board::uart::CR1::TE));
        }
    }
}