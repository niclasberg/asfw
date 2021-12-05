#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace syscfg
	{
		struct tag { };
		namespace MEMRM
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			constexpr auto MEM_MODE = reg::RWField<_Offset, reg::BitMask32<0, 2>>{ };
		}

		namespace PMC
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			constexpr auto MII_RMII_SEL = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
		}

		namespace EXTICR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			constexpr auto EXTI = reg::RWMultiField<_Offset, reg::BitMask32<0, 4>, reg::RepMask<4, 4>, reg::RepLocation<4, 0x4>>{ };
		}

		namespace CMPCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;
			constexpr auto READY = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto CMP_PD = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		}
	};
}
