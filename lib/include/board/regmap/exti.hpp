#pragma once
#include "reg/field.hpp"
#include "reg/multi_field.hpp"

namespace board
{
	namespace exti
	{
		struct tag { };
		/** Interrupt mask register          (EXTI_IMR) */
		namespace IMR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Interrupt Mask on line X */
			constexpr auto MR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};

		/** Event mask register (EXTI_EMR) */
		namespace EMR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** Event Mask on line X */
			constexpr auto MR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};

		/** Rising Trigger selection register          (EXTI_RTSR) */
		namespace RTSR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** Rising trigger event configuration of line X */
			constexpr auto TR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};

		/** Falling Trigger selection register          (EXTI_FTSR) */
		namespace FTSR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			/** Falling trigger event configuration of line X */
			constexpr auto TR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};

		/** Software interrupt event register          (EXTI_SWIER) */
		namespace SWIER
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** Software Interrupt on line X */
			constexpr auto SWIER = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};

		/** Pending register (EXTI_PR) */
		namespace PR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** Pending bit X */
			constexpr auto PR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<23, 1>, reg::RepLocation<1, 1>>{};
		};
	};
}
