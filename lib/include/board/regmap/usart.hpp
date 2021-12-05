#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace usart
	{
		struct tag { };
		/** Status register */
		namespace SR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** CTS flag */
			constexpr auto CTS = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** LIN break detection flag */
			constexpr auto LBD = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Transmit data register              empty */
			constexpr auto TXE = reg::ROField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Transmission complete */
			constexpr auto TC = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Read data register not              empty */
			constexpr auto RXNE = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** IDLE line detected */
			constexpr auto IDLE = reg::ROField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Overrun error */
			constexpr auto ORE = reg::ROField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Noise detected flag */
			constexpr auto NF = reg::ROField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Framing error */
			constexpr auto FE = reg::ROField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Parity error */
			constexpr auto PE = reg::ROField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Data register */
		namespace DR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** Data value */
			constexpr auto DR = reg::RWField<_Offset, reg::BitMask32<0, 9>>{ };
		};

		/** Baud rate register */
		namespace BRR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** mantissa of USARTDIV */
			constexpr auto DIV_Mantissa = reg::RWField<_Offset, reg::BitMask32<4, 12>>{ };
			/** fraction of USARTDIV */
			constexpr auto DIV_Fraction = reg::RWField<_Offset, reg::BitMask32<0, 4>>{ };
		};

		/** Control register 1 */
		namespace CR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** Oversampling mode */
			constexpr auto OVER8 = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** USART enable */
			constexpr auto UE = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Word length */
			constexpr auto M = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Wakeup method */
			constexpr auto WAKE = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Parity control enable */
			constexpr auto PCE = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Parity selection */
			constexpr auto PS = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** PE interrupt enable */
			constexpr auto PEIE = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** TXE interrupt enable */
			constexpr auto TXEIE = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Transmission complete interrupt              enable */
			constexpr auto TCIE = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** RXNE interrupt enable */
			constexpr auto RXNEIE = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** IDLE interrupt enable */
			constexpr auto IDLEIE = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Transmitter enable */
			constexpr auto TE = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Receiver enable */
			constexpr auto RE = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Receiver wakeup */
			constexpr auto RWU = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Send break */
			constexpr auto SBK = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Control register 2 */
		namespace CR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** LIN mode enable */
			constexpr auto LINEN = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			/** STOP bits */
			constexpr auto STOP = reg::RWField<_Offset, reg::BitMask32<12, 2>>{ };
			/** Clock enable */
			constexpr auto CLKEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Clock polarity */
			constexpr auto CPOL = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Clock phase */
			constexpr auto CPHA = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Last bit clock pulse */
			constexpr auto LBCL = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** LIN break detection interrupt              enable */
			constexpr auto LBDIE = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** lin break detection length */
			constexpr auto LBDL = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Address of the USART node */
			constexpr auto ADD = reg::RWField<_Offset, reg::BitMask32<0, 4>>{ };
		};

		/** Control register 3 */
		namespace CR3
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** One sample bit method              enable */
			constexpr auto ONEBIT = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** CTS interrupt enable */
			constexpr auto CTSIE = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** CTS enable */
			constexpr auto CTSE = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** RTS enable */
			constexpr auto RTSE = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** DMA enable transmitter */
			constexpr auto DMAT = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** DMA enable receiver */
			constexpr auto DMAR = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Smartcard mode enable */
			constexpr auto SCEN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Smartcard NACK enable */
			constexpr auto NACK = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Half-duplex selection */
			constexpr auto HDSEL = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			/** IrDA low-power */
			constexpr auto IRLP = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** IrDA mode enable */
			constexpr auto IREN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Error interrupt enable */
			constexpr auto EIE = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Guard time and prescaler          register */
		namespace GTPR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			/** Guard time value */
			constexpr auto GT = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** Prescaler value */
			constexpr auto PSC = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
		};
	};
}
