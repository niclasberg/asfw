#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace fpu_cpacr
	{
		struct tag { };
		/** Coprocessor access control register */
		namespace CPACR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** CP */
			constexpr auto CP10 = reg::RWField<_Offset, reg::BitMask32<20, 2>>{ };
			constexpr auto CP11 = reg::RWField<_Offset, reg::BitMask32<22, 2>>{ };
		};
	};
}
