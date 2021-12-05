#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace i2c
	{
		struct tag { };
		/** Control register 1 */
		namespace CR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Software reset */
			constexpr auto SWRST = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** SMBus alert */
			constexpr auto ALERT = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Packet error checking */
			constexpr auto PEC = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Acknowledge/PEC Position (for data              reception) */
			constexpr auto POS = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Acknowledge enable */
			constexpr auto ACK = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Stop generation */
			constexpr auto STOP = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Start generation */
			constexpr auto START = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Clock stretching disable (Slave              mode) */
			constexpr auto NOSTRETCH = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** General call enable */
			constexpr auto ENGC = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** PEC enable */
			constexpr auto ENPEC = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** ARP enable */
			constexpr auto ENARP = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** SMBus type */
			constexpr auto SMBTYPE = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			/** SMBus mode */
			constexpr auto SMBUS = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Peripheral enable */
			constexpr auto PE = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Control register 2 */
		namespace CR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** DMA last transfer */
			constexpr auto LAST = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** DMA requests enable */
			constexpr auto DMAEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Buffer interrupt enable */
			constexpr auto ITBUFEN = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Event interrupt enable */
			constexpr auto ITEVTEN = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Error interrupt enable */
			constexpr auto ITERREN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Peripheral clock frequency */
			constexpr auto FREQ = reg::RWField<_Offset, reg::BitMask32<0, 6>>{ };
		};

		/** Own address register 1 */
		namespace OAR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** Addressing mode (slave              mode) */
			constexpr auto ADDMODE = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Interface address */
			constexpr auto ADD10 = reg::RWField<_Offset, reg::BitMask32<8, 2>>{ };
			/** Interface address */
			constexpr auto ADD7 = reg::RWField<_Offset, reg::BitMask32<1, 7>>{ };
			/** Interface address */
			constexpr auto ADD0 = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Own address register 2 */
		namespace OAR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** Interface address */
			constexpr auto ADD2 = reg::RWField<_Offset, reg::BitMask32<1, 7>>{ };
			/** Dual addressing mode              enable */
			constexpr auto ENDUAL = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Data register */
		namespace DR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** 8-bit data register */
			constexpr auto DR = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
		};

		/** Status register 1 */
		namespace SR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** SMBus alert */
			constexpr auto SMBALERT = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Timeout or Tlow error */
			constexpr auto TIMEOUT = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			/** PEC Error in reception */
			constexpr auto PECERR = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Overrun/Underrun */
			constexpr auto OVR = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Acknowledge failure */
			constexpr auto AF = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Arbitration lost (master              mode) */
			constexpr auto ARLO = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Bus error */
			constexpr auto BERR = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Data register empty              (transmitters) */
			constexpr auto TxE = reg::ROField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Data register not empty              (receivers) */
			constexpr auto RxNE = reg::ROField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Stop detection (slave              mode) */
			constexpr auto STOPF = reg::ROField<_Offset, reg::BitMask32<4, 1>>{ };
			/** 10-bit header sent (Master              mode) */
			constexpr auto ADD10 = reg::ROField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Byte transfer finished */
			constexpr auto BTF = reg::ROField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Address sent (master mode)/matched              (slave mode) */
			constexpr auto ADDR = reg::ROField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Start bit (Master mode) */
			constexpr auto SB = reg::ROField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Status register 2 */
		namespace SR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			/** Packet error checking              register */
			constexpr auto PEC = reg::ROField<_Offset, reg::BitMask32<8, 8>>{ };
			/** Dual flag (Slave mode) */
			constexpr auto DUALF = reg::ROField<_Offset, reg::BitMask32<7, 1>>{ };
			/** SMBus host header (Slave              mode) */
			constexpr auto SMBHOST = reg::ROField<_Offset, reg::BitMask32<6, 1>>{ };
			/** SMBus device default address (Slave              mode) */
			constexpr auto SMBDEFAULT = reg::ROField<_Offset, reg::BitMask32<5, 1>>{ };
			/** General call address (Slave              mode) */
			constexpr auto GENCALL = reg::ROField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Transmitter/receiver */
			constexpr auto TRA = reg::ROField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Bus busy */
			constexpr auto BUSY = reg::ROField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Master/slave */
			constexpr auto MSL = reg::ROField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** Clock control register */
		namespace CCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x001c>>;
			/** I2C master mode selection */
			constexpr auto F_S = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Fast mode duty cycle */
			constexpr auto DUTY = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			/** Clock control register in Fast/Standard              mode (Master mode) */
			constexpr auto CCR = reg::RWField<_Offset, reg::BitMask32<0, 12>>{ };
		};

		/** TRISE register */
		namespace TRISE
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;
			/** Maximum rise time in Fast/Standard mode              (Master mode) */
			constexpr auto TRISE = reg::RWField<_Offset, reg::BitMask32<0, 6>>{ };
		};
	};
}
