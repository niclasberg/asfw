#pragma once
#include "reg/field.hpp"
#include "reg/multi_field.hpp"

namespace board
{
	namespace adc
	{
		struct tag { };
		/** status register */
		namespace SR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			/** Overrun */
			constexpr auto OVR = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Regular channel start flag */
			constexpr auto STRT = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Injected channel start              flag */
			constexpr auto JSTRT = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Injected channel end of              conversion */
			constexpr auto JEOC = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Regular channel end of              conversion */
			constexpr auto EOC = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Analog watchdog flag */
			constexpr auto AWD = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** control register 1 */
		namespace CR1
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			/** Overrun interrupt enable */
			constexpr auto OVRIE = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };

			enum class ResVal : std::uint32_t
			{
				BITS12 = 0x00,
				BITS10 = 0x01,
				BITS8 = 0x02,
				BITS6 = 0x03
			};

			/** Resolution */
			constexpr auto RES = reg::RWField<_Offset, reg::BitMask32<24, 2>, ResVal>{ };
			/** Analog watchdog enable on regular channels */
			constexpr auto AWDEN = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			/** Analog watchdog enable on injected channels */
			constexpr auto JAWDEN = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			/** Discontinuous mode channel count */
			constexpr auto DISCNUM = reg::RWField<_Offset, reg::BitMask32<13, 3>>{ };
			/** Discontinuous mode on injected              channels */
			constexpr auto JDISCEN = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Discontinuous mode on regular              channels */
			constexpr auto DISCEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Automatic injected group              conversion */
			constexpr auto JAUTO = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Enable the watchdog on a single channel              in scan mode */
			constexpr auto AWDSGL = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Scan mode */
			constexpr auto SCAN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Interrupt enable for injected              channels */
			constexpr auto JEOCIE = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Analog watchdog interrupt              enable */
			constexpr auto AWDIE = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Interrupt enable for EOC */
			constexpr auto EOCIE = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };

			enum class AwdchVal : std::uint32_t
			{
				CHANNEL0 = 0x00,
				CHANNEL1 = 0x01,
				CHANNEL2 = 0x02,
				CHANNEL3 = 0x03,
				CHANNEL4 = 0x04,
				CHANNEL5 = 0x05,
				CHANNEL6 = 0x06,
				CHANNEL7 = 0x07,
				CHANNEL8 = 0x08,
				CHANNEL9 = 0x09,
				CHANNEL10 = 0x0A,
				CHANNEL11 = 0x0B,
				CHANNEL12 = 0x0C,
				CHANNEL13 = 0x0D,
				CHANNEL14 = 0x0E,
				CHANNEL15 = 0x0F,
				CHANNEL16 = 0x10,
				CHANNEL17 = 0x11,
				CHANNEL18 = 0x12,
			};

			/** Analog watchdog channel select              bits */
			constexpr auto AWDCH = reg::RWField<_Offset, reg::BitMask32<0, 5>, AwdchVal>{ };
		};

		/** control register 2 */
		namespace CR2
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			/** Start conversion of regular              channels */
			constexpr auto SWSTART = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };

			enum class ExtEnVal : std::uint32_t
			{
				DISABLED = 0x00,
				RISING_EDGE = 0x01,
				FALLING_EDGE = 0x02,
				RISING_FALLING_EDGE = 0x03
			};

			/** External trigger enable for regular              channels */
			constexpr auto EXTEN = reg::RWField<_Offset, reg::BitMask32<28, 2>, ExtEnVal>{ };

			enum class ExtSelVal : std::uint32_t
			{
				TIMER1_CC1 = 0x00,
				TIMER1_CC2 = 0x01,
				TIMER1_CC3 = 0x02,
				TIMER2_CC2 = 0x03,
				TIMER2_CC3 = 0x04,
				TIMER2_CC4 = 0x05,
				TIMER2_TRGO = 0x06,
				TIMER3_CC1 = 0x07,
				TIMER3_TRGO = 0x08,
				TIMER4_CC4 = 0x09,
				TIMER5_CC1 = 0x0A,
				TIMER5_CC2 = 0x0B,
				TIMER5_CC3 = 0x0C,
				TIMER8_CC1 = 0x0D,
				TIMER8_TRGO = 0x0E,
				EXTI_LINE11 = 0x0F,
			};

			/** External event select for regular              group */
			constexpr auto EXTSEL = reg::RWField<_Offset, reg::BitMask32<24, 4>, ExtSelVal>{ };
			/** Start conversion of injected              channels */
			constexpr auto JSWSTART = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };

			enum class JExtEnVal : std::uint32_t
			{
				DISABLED = 0x00,
				RISING_EDGE = 0x01,
				FALLING_EDGE = 0x02,
				RISING_FALLING_EDGE = 0x03
			};

			/** External trigger enable for injected              channels */
			constexpr auto JEXTEN = reg::RWField<_Offset, reg::BitMask32<20, 2>, JExtEnVal>{ };
			/** External event select for injected              group */
			constexpr auto JEXTSEL = reg::RWField<_Offset, reg::BitMask32<16, 4>>{ };

			enum class AlignVal : std::uint32_t
			{
				RIGHT,
				LEFT
			};

			/** Data alignment */
			constexpr auto ALIGN = reg::RWField<_Offset, reg::BitMask32<11, 1>, AlignVal>{ };
			/** End of conversion              selection */
			constexpr auto EOCS = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** DMA disable selection (for single ADC              mode) */
			constexpr auto DDS = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Direct memory access mode (for single              ADC mode) */
			constexpr auto DMA = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Continuous conversion */
			constexpr auto CONT = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** A/D Converter ON / OFF */
			constexpr auto ADON = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** sample time register */
		namespace SMPR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x00c>>;
			using _Offset2 = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x010>>;

			enum class SmprVal : std::uint32_t
			{
				DIV3 = 0x00,
				DIV15 = 0x01,
				DIV28 = 0x02,
				DIV56 = 0x03,
				DIV84 = 0x04,
				DIV112 = 0x05,
				DIV144 = 0x06,
				DIV480 = 0x07
			};

			/** Sample time bits */
			constexpr auto SMP = hana::make_tuple(
				reg::RWField<_Offset2, reg::BitMask32<0, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<3, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<6, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<9, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<12, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<15, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<18, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<21, 2>, SmprVal>{},
				reg::RWField<_Offset2, reg::BitMask32<24, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<0, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<3, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<6, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<9, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<12, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<15, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<18, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<21, 2>, SmprVal>{},
				reg::RWField<_Offset, reg::BitMask32<24, 2>, SmprVal>{});
		};

		/** injected channel data offset register          x */
		namespace JOFR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** Data offset for injected channel x */
			constexpr auto JOFFSET = reg::RWMultiField<_Offset, reg::BitMask32<0, 12>, reg::RepMask<1, 1>, reg::RepLocation<5, 0x04>>{ };
		};

		/** watchdog higher threshold register */
		namespace HTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0024>>;
			/** Analog watchdog higher              threshold */
			constexpr auto HT = reg::RWField<_Offset, reg::BitMask32<0, 12>>{ };
		};

		/** watchdog lower threshold register */
		namespace LTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0028>>;
			/** Analog watchdog lower              threshold */
			constexpr auto LT = reg::RWField<_Offset, reg::BitMask32<0, 12>>{ };
		};

		/** regular sequence register */
		namespace SQR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x002c>>;
			using _Offset2 = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0030>>;
			using _Offset3 = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0034>>;
			
			/** Regular channel sequence length */
			constexpr auto L = reg::RWField<_Offset, reg::BitMask32<20, 4>>{ };

			/** nth conversion in regular sequence */
			constexpr auto SQ = hana::make_tuple(
				reg::RWField<_Offset3, reg::BitMask32<0, 5>>{ },
				reg::RWField<_Offset3, reg::BitMask32<5, 5>>{ },
				reg::RWField<_Offset3, reg::BitMask32<10, 5>>{ },
				reg::RWField<_Offset3, reg::BitMask32<15, 5>>{ },
				reg::RWField<_Offset3, reg::BitMask32<20, 5>>{ },
				reg::RWField<_Offset3, reg::BitMask32<25, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<0, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<5, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<10, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<15, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<20, 5>>{ },
				reg::RWField<_Offset2, reg::BitMask32<25, 5>>{ },
				reg::RWField<_Offset, reg::BitMask32<0, 5>>{ },
				reg::RWField<_Offset, reg::BitMask32<5, 5>>{ },
				reg::RWField<_Offset, reg::BitMask32<10, 5>>{ },
				reg::RWField<_Offset, reg::BitMask32<15, 5>>{ }
			);
		};

		/** injected sequence register */
		namespace JSQR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0038>>;
			/** Injected sequence length */
			constexpr auto JL = reg::RWField<_Offset, reg::BitMask32<20, 2>>{ };
			/** nth conversion in injected sequence */
			constexpr auto JSQ = reg::RWMultiField<_Offset, reg::BitMask32<0, 5>, reg::RepMask<4, 5>, reg::RepLocation<1, 0>>{ };
		};

		/** injected data register x */
		namespace JDR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x003c>>;
			/** Injected data */
			constexpr auto JDATA = reg::ROMultiField<_Offset, reg::BitMask32<0, 16>, reg::RepMask<1, 0>, reg::RepLocation<4, 0x4>>{ };
		};

		/** regular data register */
		namespace DR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x004c>>;
			/** Regular data */
			constexpr auto DATA = reg::ROField<_Offset, reg::BitMask32<0, 16>>{ };
		};
	};
}
