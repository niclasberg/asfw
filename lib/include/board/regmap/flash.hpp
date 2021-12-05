#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace flash
	{
		struct tag { };
		/** Flash access control register */
		namespace ACR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Latency */
			constexpr auto LATENCY = reg::RWField<_Offset, reg::BitMask32<0, 3>>{ };
			/** Prefetch enable */
			constexpr auto PRFTEN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Instruction cache enable */
			constexpr auto ICEN = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Data cache enable */
			constexpr auto DCEN = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Instruction cache reset */
			constexpr auto ICRST = reg::WOField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Data cache reset */
			constexpr auto DCRST = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
		};

		/** Flash key register */
		namespace KEYR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** FPEC key */
			constexpr auto KEY = reg::WOField<_Offset, reg::BitMask32<0, 32>>{ };
		};

		/** Flash option key register */
		namespace OPTKEYR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** Option byte key */
			constexpr auto OPTKEY = reg::WOField<_Offset, reg::BitMask32<0, 32>>{ };
		};

		/** Status register */
		namespace SR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** End of operation */
			constexpr auto EOP = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
			/** Operation error */
			constexpr auto OPERR = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Write protection error */
			constexpr auto WRPERR = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Programming alignment              error */
			constexpr auto PGAERR = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Programming parallelism              error */
			constexpr auto PGPERR = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Programming sequence error */
			constexpr auto PGSERR = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Busy */
			constexpr auto BSY = reg::ROField<_Offset, reg::BitMask32<16, 1>>{ };
		};

		/** Control register */
		namespace CR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** Programming */
			constexpr auto PG = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
			/** Sector Erase */
			constexpr auto SER = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Mass Erase */
			constexpr auto MER = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Sector number */
			constexpr auto SNB = reg::RWField<_Offset, reg::BitMask32<3, 4>>{ };
			/** Program size */
			constexpr auto PSIZE = reg::RWField<_Offset, reg::BitMask32<8, 2>>{ };
			/** Start */
			constexpr auto STRT = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			/** End of operation interrupt              enable */
			constexpr auto EOPIE = reg::RWField<_Offset, reg::BitMask32<24, 1>>{ };
			/** Error interrupt enable */
			constexpr auto ERRIE = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			/** Lock */
			constexpr auto LOCK = reg::RWField<_Offset, reg::BitMask32<31, 1>>{ };
		};

		/** Flash option control register */
		namespace OPTCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** Option lock */
			constexpr auto OPTLOCK = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
			/** Option start */
			constexpr auto OPTSTRT = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** BOR reset Level */
			constexpr auto BOR_LEV = reg::RWField<_Offset, reg::BitMask32<2, 2>>{ };
			/** WDG_SW User option bytes */
			constexpr auto WDG_SW = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** nRST_STOP User option              bytes */
			constexpr auto nRST_STOP = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** nRST_STDBY User option              bytes */
			constexpr auto nRST_STDBY = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Read protect */
			constexpr auto RDP = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** Not write protect */
			constexpr auto nWRP = reg::RWField<_Offset, reg::BitMask32<16, 12>>{ };
		};
	};
}
