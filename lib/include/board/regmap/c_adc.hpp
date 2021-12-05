#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace c_adc
	{
		struct tag { };
		/** ADC Common status register */
		namespace CSR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Overrun flag of ADC3 */
			constexpr auto OVR3 = reg::ROField<_Offset, reg::BitMask32<21, 1>>{ };
			/** Regular channel Start flag of ADC              3 */
			constexpr auto STRT3 = reg::ROField<_Offset, reg::BitMask32<20, 1>>{ };
			/** Injected channel Start flag of ADC              3 */
			constexpr auto JSTRT3 = reg::ROField<_Offset, reg::BitMask32<19, 1>>{ };
			/** Injected channel end of conversion of              ADC 3 */
			constexpr auto JEOC3 = reg::ROField<_Offset, reg::BitMask32<18, 1>>{ };
			/** End of conversion of ADC 3 */
			constexpr auto EOC3 = reg::ROField<_Offset, reg::BitMask32<17, 1>>{ };
			/** Analog watchdog flag of ADC              3 */
			constexpr auto AWD3 = reg::ROField<_Offset, reg::BitMask32<16, 1>>{ };
			/** Overrun flag of ADC 2 */
			constexpr auto OVR2 = reg::ROField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Regular channel Start flag of ADC              2 */
			constexpr auto STRT2 = reg::ROField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Injected channel Start flag of ADC              2 */
			constexpr auto JSTRT2 = reg::ROField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Injected channel end of conversion of              ADC 2 */
			constexpr auto JEOC2 = reg::ROField<_Offset, reg::BitMask32<10, 1>>{ };
			/** End of conversion of ADC 2 */
			constexpr auto EOC2 = reg::ROField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Analog watchdog flag of ADC              2 */
			constexpr auto AWD2 = reg::ROField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Overrun flag of ADC 1 */
			constexpr auto OVR1 = reg::ROField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Regular channel Start flag of ADC              1 */
			constexpr auto STRT1 = reg::ROField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Injected channel Start flag of ADC              1 */
			constexpr auto JSTRT1 = reg::ROField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Injected channel end of conversion of              ADC 1 */
			constexpr auto JEOC1 = reg::ROField<_Offset, reg::BitMask32<2, 1>>{ };
			/** End of conversion of ADC 1 */
			constexpr auto EOC1 = reg::ROField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Analog watchdog flag of ADC              1 */
			constexpr auto AWD1 = reg::ROField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** ADC common control register */
		namespace CCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** Temperature sensor and VREFINT              enable */
			constexpr auto TSVREFE = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			/** VBAT enable */
			constexpr auto VBATE = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			/** ADC prescaler */
			constexpr auto ADCPRE = reg::RWField<_Offset, reg::BitMask32<16, 2>>{ };
			/** Direct memory access mode for multi ADC              mode */
			constexpr auto DMA = reg::RWField<_Offset, reg::BitMask32<14, 2>>{ };
			/** DMA disable selection for multi-ADC              mode */
			constexpr auto DDS = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Delay between 2 sampling              phases */
			constexpr auto DELAY = reg::RWField<_Offset, reg::BitMask32<8, 4>>{ };
			/** Multi ADC mode selection */
			constexpr auto MULT = reg::RWField<_Offset, reg::BitMask32<0, 5>>{ };
		};

		/** ADC common regular data register for dual          and triple modes */
		namespace CDR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** 2nd data item of a pair of regular              conversions */
			constexpr auto DATA2 = reg::ROField<_Offset, reg::BitMask32<16, 16>>{ };
			/** 1st data item of a pair of regular              conversions */
			constexpr auto DATA1 = reg::ROField<_Offset, reg::BitMask32<0, 16>>{ };
		};
	};
}
