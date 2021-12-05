#pragma once
#include "reg/field.hpp"
#include "reg/multi_field.hpp"

namespace board
{
	namespace dma
	{
		struct tag { };
		/** interrupt status register */
		namespace ISR
		{
			using _OffsetLow = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			using _OffsetHigh = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;

			/** Stream x transfer complete interrupt flag */
			constexpr auto TCIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<5, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<11, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<21, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<27, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<5, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<11, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<21, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<27, 1>>{ });

			/** Stream x half transfer interrupt flag */
			constexpr auto HTIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<4, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<10, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<20, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<26, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<4, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<10, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<20, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<26, 1>>{ });

			/** Stream x transfer error interrupt flag */
			constexpr auto TEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<3, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<9, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<19, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<25, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<3, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<9, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<19, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<25, 1>>{ });

			/** Stream x direct mode error interrupt flag */
			constexpr auto DMEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<2, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<8, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<18, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<24, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<2, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<8, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<18, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<24, 1>>{ });
			
			/** Stream x FIFO error interrupt flag */
			constexpr auto FEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<0, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<6, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<16, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<22, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<0, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<6, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<16, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<22, 1>>{ });
		}

		namespace IFCR
		{
			using _OffsetLow = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			using _OffsetHigh = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;

			/** Stream x clear transfer complete interrupt flag */
			constexpr auto CTCIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<5, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<11, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<21, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<27, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<5, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<11, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<21, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<27, 1>>{ });

			/** Stream x clear half transfer interrupt flag */
			constexpr auto CHTIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<4, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<10, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<20, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<26, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<4, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<10, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<20, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<26, 1>>{ });

			/** Stream x clear transfer error interrupt flag */
			constexpr auto CTEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<3, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<9, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<19, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<25, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<3, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<9, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<19, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<25, 1>>{ });

			/** Stream x clear direct mode error interrupt flag */
			constexpr auto CDMEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<2, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<8, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<18, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<24, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<2, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<8, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<18, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<24, 1>>{ });
			
			constexpr auto CFEIF = hana::make_tuple(
				reg::RWField<_OffsetLow, reg::BitMask32<0, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<6, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<16, 1>>{ },
				reg::RWField<_OffsetLow, reg::BitMask32<22, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<0, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<6, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<16, 1>>{ },
				reg::RWField<_OffsetHigh, reg::BitMask32<22, 1>>{ });
		}

		namespace CR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;

			enum class ChselVal : std::uint32_t
			{
				CHANNEL0 = 0x00, 
				CHANNEL1 = 0x01, 
				CHANNEL2 = 0x02, 
				CHANNEL3 = 0x03, 
				CHANNEL4 = 0x04, 
				CHANNEL5 = 0x05, 
				CHANNEL6 = 0x06, 
				CHANNEL7 = 0x07
			};

			constexpr auto CHSEL = reg::RWMultiField<_Offset, reg::BitMask32<25, 3>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>, ChselVal>{ };
			constexpr auto MBURST = reg::RWMultiField<_Offset, reg::BitMask32<23, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };

			enum class MburstVal : std::uint32_t
			{
				SINGLE_TRANSFER = 0x00,
				INCR4 = 0x01,
				INCR8 = 0x02,
				INCR16 = 0x03
			};

			constexpr auto PBURST = reg::RWMultiField<_Offset, reg::BitMask32<21, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };

			enum class PburstVal : std::uint32_t
			{
				SINGLE_TRANSFER = 0x00,
				INCR4 = 0x01,
				INCR8 = 0x02,
				INCR16 = 0x03
			};

			constexpr auto CT = reg::RWMultiField<_Offset, reg::BitMask32<19, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto DBM = reg::RWMultiField<_Offset, reg::BitMask32<18, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			
			enum class PlVal : std::uint32_t
			{
				LOW = 0x00,
				MEDIUM = 0x01,
				HIGH = 0x02,
				VERY_HIGH = 0x03
			};

			constexpr auto PL = reg::RWMultiField<_Offset, reg::BitMask32<16, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>, PlVal>{ };

			constexpr auto PINCOS = reg::RWMultiField<_Offset, reg::BitMask32<15, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto MSIZE = reg::RWMultiField<_Offset, reg::BitMask32<13, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };

			enum class MsizeVal : std::uint32_t
			{
				BYTE = 0x00,
				HALF_WORD = 0x01,
				WORD = 0x02
			};

			constexpr auto PSIZE = reg::RWMultiField<_Offset, reg::BitMask32<11, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };

			enum class PsizeVal : std::uint32_t
			{
				BYTE = 0x00,
				HALF_WORD = 0x01,
				WORD = 0x02
			};

			constexpr auto MINC = reg::RWMultiField<_Offset, reg::BitMask32<10, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto PINC = reg::RWMultiField<_Offset, reg::BitMask32<9, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto CIRC = reg::RWMultiField<_Offset, reg::BitMask32<8, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			
			enum class DirVal : std::uint32_t
			{
				PERIPHERAL_TO_MEMORY = 0x00,
				MEMORY_TO_PERIPHERAL = 0x01,
				MEMORY_TO_MEMORY = 0x02,
			};
			
			constexpr auto DIR = reg::RWMultiField<_Offset, reg::BitMask32<6, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>, DirVal>{ };

			constexpr auto PFCTRL = reg::RWMultiField<_Offset, reg::BitMask32<5, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto TCIE = reg::RWMultiField<_Offset, reg::BitMask32<4, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto HTIE = reg::RWMultiField<_Offset, reg::BitMask32<3, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto TEIE = reg::RWMultiField<_Offset, reg::BitMask32<2, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto DMEIE = reg::RWMultiField<_Offset, reg::BitMask32<1, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto EN = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};

		namespace NDTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			constexpr auto NDT = reg::RWMultiField<_Offset, reg::BitMask32<0, 16>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};

		namespace PAR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			constexpr auto PA = reg::RWMultiField<_Offset, reg::BitMask32<0, 32>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};

		namespace M0AR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x001c>>;
			constexpr auto M0A = reg::RWMultiField<_Offset, reg::BitMask32<0, 32>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};

		namespace M1AR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;
			constexpr auto M1A = reg::RWMultiField<_Offset, reg::BitMask32<0, 32>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};

		namespace FCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0024>>;
			constexpr auto FEIE = reg::RWMultiField<_Offset, reg::BitMask32<7, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto FS = reg::RWMultiField<_Offset, reg::BitMask32<3, 3>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto DMDIS = reg::RWMultiField<_Offset, reg::BitMask32<2, 1>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
			constexpr auto FTH = reg::RWMultiField<_Offset, reg::BitMask32<0, 2>, reg::RepMask<1, 1>, reg::RepLocation<8, 0x18>>{ };
		};
	};
}
