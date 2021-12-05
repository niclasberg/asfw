#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace stk
	{
		struct tag { };
		/** SysTick control and status          register */
		namespace CTRL
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Counter enable */
			constexpr auto ENABLE = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
			/** SysTick exception request              enable */
			constexpr auto TICKINT = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };

			enum class ClkSourceVal : uint32_t {
				EXTERNAL = 0x00,
				PROCESSOR = 0x01
			};

			/** Clock source selection */
			constexpr auto CLKSOURCE = reg::RWField<_Offset, reg::BitMask32<2, 1>, ClkSourceVal>{ };
			/** COUNTFLAG */
			constexpr auto COUNTFLAG = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
		};

		/** SysTick reload value register */
		namespace LOAD
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** RELOAD value */
			constexpr auto RELOAD = reg::RWField<_Offset, reg::BitMask32<0, 24>>{ };
		};

		/** SysTick current value register */
		namespace VAL
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** Current counter value */
			constexpr auto CURRENT = reg::RWField<_Offset, reg::BitMask32<0, 24>>{ };
		};

		/** SysTick calibration value          register */
		namespace CALIB
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** Calibration value */
			constexpr auto TENMS = reg::RWField<_Offset, reg::BitMask32<0, 24>>{ };
			/** SKEW flag: Indicates whether the TENMS              value is exact */
			constexpr auto SKEW = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };
			/** NOREF flag. Reads as zero */
			constexpr auto NOREF = reg::RWField<_Offset, reg::BitMask32<31, 1>>{ };
		};
	};
}
