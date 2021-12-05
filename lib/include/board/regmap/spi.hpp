#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace spi
	{
		struct tag { };
		/** control register 1 */
		namespace CR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Bidirectional data mode enable */
			constexpr auto BIDIMODE = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Output enable in bidirectional mode */
			constexpr auto BIDIOE = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			/** Hardware CRC calculation enable */
			constexpr auto CRCEN = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			/** CRC transfer next */
			constexpr auto CRCNEXT = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };

			enum class DffVal : uint32_t {
				BYTE = 0x00,
				HALF_WORD = 0x01
			};

			/** Data frame format */
			constexpr auto DFF = reg::RWField<_Offset, reg::BitMask32<11, 1>, DffVal>{ };
			/** Receive only */
			constexpr auto RXONLY = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Software slave management */
			constexpr auto SSM = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Internal slave select */
			constexpr auto SSI = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };	
			/** Frame format */
			constexpr auto LSBFIRST = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** SPI enable */
			constexpr auto SPE = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };

			enum class BrVal : uint32_t {
				PCKL_DIV2 = 0x00,
				PCKL_DIV4 = 0x01,
				PCKL_DIV8 = 0x02,
				PCKL_DIV16 = 0x03,
				PCKL_DIV32 = 0x04,
				PCKL_DIV64 = 0x05,
				PCKL_DIV128 = 0x06,
				PCKL_DIV256 = 0x07
			};

			/** Baud rate control */
			constexpr auto BR = reg::RWField<_Offset, reg::BitMask32<3, 3>, BrVal>{ };
			/** Master selection */
			constexpr auto MSTR = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };

			enum class CpolVal : std::uint32_t
			{
				LOW = 0x00,
				HIGH = 0x01
			};

			/** Clock polarity */
			constexpr auto CPOL = reg::RWField<_Offset, reg::BitMask32<1, 1>, CpolVal>{ };

			enum class CphaVal : std::uint32_t
			{
				LOW = 0x00,
				HIGH = 0x01
			};

			/** Clock phase */
			constexpr auto CPHA = reg::RWField<_Offset, reg::BitMask32<0, 1>, CphaVal>{ };
		};

		/** control register 2 */
		namespace CR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** Tx buffer empty interrupt enable */
			constexpr auto TXEIE = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** RX buffer not empty interrupt enable */
			constexpr auto RXNEIE = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Error interrupt enable */
			constexpr auto ERRIE = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Frame format */
			constexpr auto FRF = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** SS output enable */
			constexpr auto SSOE = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Tx buffer DMA enable */
			constexpr auto TXDMAEN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Rx buffer DMA enable */
			constexpr auto RXDMAEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** status register */
		namespace SR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** TI frame format error */
			constexpr auto TIFRFE = reg::ROField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Busy flag */
			constexpr auto BSY = reg::ROField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Overrun flag */
			constexpr auto OVR = reg::ROField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Mode fault */
			constexpr auto MODF = reg::ROField<_Offset, reg::BitMask32<5, 1>>{ };
			/** CRC error flag */
			constexpr auto CRCERR = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Underrun flag */
			constexpr auto UDR = reg::ROField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Channel side */
			constexpr auto CHSIDE = reg::ROField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Transmit buffer empty */
			constexpr auto TXE = reg::ROField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Receive buffer not empty */
			constexpr auto RXNE = reg::ROField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** data register */
		namespace DR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** Data register */
			constexpr auto DR = reg::RWField<_Offset, reg::BitMask32<0, 16>>{ };
		};

		/** CRC polynomial register */
		namespace CRCPR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** CRC polynomial register */
			constexpr auto CRCPOLY = reg::RWField<_Offset, reg::BitMask32<0, 16>>{ };
		};

		/** RX CRC register */
		namespace RXCRCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** Rx CRC register */
			constexpr auto RxCRC = reg::ROField<_Offset, reg::BitMask32<0, 16>>{ };
		};

		/** TX CRC register */
		namespace TXCRCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			/** Tx CRC register */
			constexpr auto TxCRC = reg::ROField<_Offset, reg::BitMask32<0, 16>>{ };
		};

		/** I2S configuration register */
		namespace I2SCFGR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x001c>>;

			/** I2S mode selection */
			constexpr auto I2SMOD = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** I2S Enable */
			constexpr auto I2SE = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };

			enum class I2sCfgVal : std::uint32_t
			{
				SLAVE_TX = 0x00,
				SLAVE_RX = 0x01,
				MASTER_TX = 0x02,
				MASTER_RX = 0x03
			};

			/** I2S configuration mode */
			constexpr auto I2SCFG = reg::RWField<_Offset, reg::BitMask32<8, 2>, I2sCfgVal>{ };
			/** PCM frame synchronization */
			constexpr auto PCMSYNC = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };

			enum class I2sStdVal : std::uint32_t
			{
				PHILIPS = 0x00,
				MSB = 0x01,
				LSB = 0x02,
				PCM = 0x03
			};

			/** I2S standard selection */
			constexpr auto I2SSTD = reg::RWField<_Offset, reg::BitMask32<4, 2>, I2sStdVal>{ };

			enum class CkPolVal : std::uint32_t
			{
				LOW = 0x00,
				HIGH = 0x01
			};

			/** Steady state clock polarity */
			constexpr auto CKPOL = reg::RWField<_Offset, reg::BitMask32<3, 1>, CkPolVal>{ };

			enum class DataLenVal : std::uint32_t
			{
				BITS16 = 0x00,
				BITS24 = 0x01,
				BITS32 = 0x02,
			};

			/** Data length to be transferred */
			constexpr auto DATLEN = reg::RWField<_Offset, reg::BitMask32<1, 2>, DataLenVal>{ };

			enum class ChLenVal : std::uint32_t
			{
				BITS16 = 0x00,
				BITS32 = 0x01
			};

			/** Channel length (number of bits per audio channel) */
			constexpr auto CHLEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** I2S prescaler register */
		namespace I2SPR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;
			/** Master clock output enable */
			constexpr auto MCKOE = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Odd factor for the              prescaler */
			constexpr auto ODD = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** I2S Linear prescaler */
			constexpr auto I2SDIV = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
		};
	};
}
