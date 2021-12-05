#pragma once
#include "reg/field.hpp"
#include "reg/multi_field.hpp"

namespace board
{
	namespace nvic
	{
		struct tag { };
		/** Interrupt Set-Enable Register */
		namespace ISER
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			constexpr auto SETENA = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<32, 1>, reg::RepLocation<8, 0x4>>{ };
		};

		/** Interrupt Clear-Enable          Register */
		namespace ICER
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0080>>;
			constexpr auto CLRENA = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<32, 1>, reg::RepLocation<8, 0x4>>{ };
		};

		/** Interrupt Set-Pending Register */
		namespace ISPR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0100>>;
			constexpr auto SETPEND = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<32, 1>, reg::RepLocation<8, 0x4>>{ };
		};

		/** Interrupt Clear-Pending          Register */
		namespace ICPR0
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0180>>;
			/** CLRPEND */
			constexpr auto CLRPEND = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<32, 1>, reg::RepLocation<8, 0x4>>{ };
		};

		/** Interrupt Active Bit Register */
		namespace IABR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0200>>;
			/** ACTIVE */
			constexpr auto ACTIVE = reg::ROMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<32, 1>, reg::RepLocation<8, 0x4>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR0
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0300>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0304>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0308>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR3
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x030c>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR4
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0310>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR5
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0314>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR6
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0318>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR7
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x031c>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR8
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0320>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR9
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0324>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR10
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0328>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR11
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x032c>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR12
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0330>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR13
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0334>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR14
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0338>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR15
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x033c>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR16
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0340>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR17
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0344>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR18
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0348>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};

		/** Interrupt Priority Register */
		namespace IPR19
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x034c>>;
			/** IPR_N0 */
			constexpr auto IPR_N0 = reg::RWField<_Offset, reg::BitMask32<0, 8>>{ };
			/** IPR_N1 */
			constexpr auto IPR_N1 = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			/** IPR_N2 */
			constexpr auto IPR_N2 = reg::RWField<_Offset, reg::BitMask32<16, 8>>{ };
			/** IPR_N3 */
			constexpr auto IPR_N3 = reg::RWField<_Offset, reg::BitMask32<24, 8>>{ };
		};
	};
}
