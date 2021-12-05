#pragma once
#include "reg/field.hpp"
#include "reg/multi_field.hpp"

namespace board
{
	namespace gpio
	{
		struct tag { };
		/** GPIO port mode register */
		namespace MODER
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;

			enum class ModerVal : std::uint32_t
			{
				INPUT = 0x00,
				OUTPUT = 0x01,
				ALTERNATE_FUNCTION = 0x02,
				ANALOG = 0x03
			};

			/** Port x configuration bits (y =              0..15) */
			constexpr auto MODER = reg::RWMultiField<_Offset, reg::BitMask32<0, 2>, reg::RepMask<16, 2>, reg::RepLocation<1, 1>, ModerVal> { };
		};

		/** GPIO port output type register */
		namespace OTYPER
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;

			enum class OtyperVal : std::uint32_t
			{
				PUSH_PULL = 0x0,
				OPEN_DRAIN = 0x1
			};

			/** Port x configuration bits (y =              0..15) */
			constexpr auto OT = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<16, 1>, reg::RepLocation<1,1>, OtyperVal>{ };
		};

		/** GPIO port output speed          register */
		namespace OSPEEDR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;

			enum class OspeedrVal : std::uint32_t
			{
				LOW = 0x0,
				MEDIUM = 0x1,
				HIGH = 0x2,
				VERY_HIGH = 0x3
			};

			/** Port x configuration bits (y =              0..15) */
			constexpr auto OSPEEDR = reg::RWMultiField<_Offset, reg::BitMask32<0, 2>, reg::RepMask<16, 2>, reg::RepLocation<1,1>, OspeedrVal>{ };
		};

		/** GPIO port pull-up/pull-down          register */
		namespace PUPDR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;

			enum class PupdrVal : std::uint32_t
			{
				NO_PULL = 0x0,
				PULL_UP = 0x1,
				PULL_DOWN = 0x2
			};

			constexpr auto PUPDR = reg::RWMultiField<_Offset, reg::BitMask32<0, 2>, reg::RepMask<16, 2>, reg::RepLocation<1,1>, PupdrVal>{ };
		};

		/** GPIO port input data register */
		namespace IDR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			/** Port input data (y =              0..15) */
			constexpr auto IDR = reg::ROMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<16, 1>, reg::RepLocation<1,1>>{ };
		};

		/** GPIO port output data register */
		namespace ODR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			/** Port output data (y =              0..15) */
			constexpr auto ODR = reg::RWMultiField<_Offset, reg::BitMask32<0, 1>, reg::RepMask<16, 1>, reg::RepLocation<1,1>>{ };
		};

		/** GPIO port bit set/reset          register */
		namespace BSRR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR15 = reg::WOField<_Offset, reg::BitMask32<31, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR14 = reg::WOField<_Offset, reg::BitMask32<30, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR13 = reg::WOField<_Offset, reg::BitMask32<29, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR12 = reg::WOField<_Offset, reg::BitMask32<28, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR11 = reg::WOField<_Offset, reg::BitMask32<27, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR10 = reg::WOField<_Offset, reg::BitMask32<26, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR9 = reg::WOField<_Offset, reg::BitMask32<25, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR8 = reg::WOField<_Offset, reg::BitMask32<24, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR7 = reg::WOField<_Offset, reg::BitMask32<23, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR6 = reg::WOField<_Offset, reg::BitMask32<22, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR5 = reg::WOField<_Offset, reg::BitMask32<21, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR4 = reg::WOField<_Offset, reg::BitMask32<20, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR3 = reg::WOField<_Offset, reg::BitMask32<19, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR2 = reg::WOField<_Offset, reg::BitMask32<18, 1>>{ };
			/** Port x reset bit y (y =              0..15) */
			constexpr auto BR1 = reg::WOField<_Offset, reg::BitMask32<17, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BR0 = reg::WOField<_Offset, reg::BitMask32<16, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS15 = reg::WOField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS14 = reg::WOField<_Offset, reg::BitMask32<14, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS13 = reg::WOField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS12 = reg::WOField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS11 = reg::WOField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS10 = reg::WOField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS9 = reg::WOField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS8 = reg::WOField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS7 = reg::WOField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS6 = reg::WOField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS5 = reg::WOField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS4 = reg::WOField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS3 = reg::WOField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS2 = reg::WOField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS1 = reg::WOField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Port x set bit y (y=              0..15) */
			constexpr auto BS0 = reg::WOField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** GPIO port configuration lock          register */
		namespace LCKR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x001c>>;
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCKK = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK15 = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK14 = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK13 = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK12 = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK11 = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK10 = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK9 = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK8 = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK7 = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK6 = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK5 = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK4 = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK3 = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK2 = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK1 = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			/** Port x lock bit y (y=              0..15) */
			constexpr auto LCK0 = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		/** GPIO alternate function register */
		namespace AFR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;

			enum class AfrVal : std::uint32_t
			{
				SYSTEM = 0x0,
				TIMER1_2 = 0x1,
				TIMER3_5 = 0x2,
				TIMER8_11 = 0x3, 
				I2C = 0x4,
				SPI1_2 = 0x5,
				SPI3 = 0x6,
				USART1_3 = 0x7,
				USART4_6 = 0x8,
				CAN1_2_TIMER_12_14 = 0x9,
				OTG_FS_HS = 0xA,
				ETH = 0xB,
				FSMC_SDIO_OTG_HS = 0xC,
				DCMI = 0xD,
				EVENTOUT = 0xF
			};

			constexpr auto AFR = reg::RWMultiField<_Offset, reg::BitMask32<0, 4>, reg::RepMask<8, 4>, reg::RepLocation<2, 0x04>, AfrVal>{};
		}
	};
}
