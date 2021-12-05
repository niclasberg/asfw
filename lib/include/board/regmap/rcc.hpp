#pragma once
#include "reg/field.hpp"

namespace board
{
	namespace rcc
	{
		struct tag { };
		namespace CR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0000>>;
			constexpr auto PLLI2SRDY = reg::RWField<_Offset, reg::BitMask32<27, 1>>{ };
			constexpr auto PLLI2SON = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto PLLRDY = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto PLLON = reg::RWField<_Offset, reg::BitMask32<24, 1>>{ };
			constexpr auto CSSON = reg::RWField<_Offset, reg::BitMask32<19, 1>>{ };
			constexpr auto HSEBYP = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto HSERDY = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto HSEON = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto HSICAL = reg::RWField<_Offset, reg::BitMask32<8, 8>>{ };
			constexpr auto HSITRIM = reg::RWField<_Offset, reg::BitMask32<3, 5>>{ };
			constexpr auto HSIRDY = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto HSION = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace PLLCFGR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0004>>;
			constexpr auto PLLQ = reg::RWField<_Offset, reg::BitMask32<24, 4>>{ };

			enum class PllSrcVal : std::uint32_t
			{
				HSI = 0,
				HSE = 1
			};

			constexpr auto PLLSRC = reg::RWField<_Offset, reg::BitMask32<22, 1>, PllSrcVal>{ };

			enum class PllPVal : std::uint32_t
			{
				DIV2 = 0x0,
				DIV4 = 0x1,
				DIV6 = 0x2,
				DIV8 = 0x3
			};

			constexpr auto PLLP = reg::RWField<_Offset, reg::BitMask32<16, 2>, PllPVal>{ };
			constexpr auto PLLN = reg::RWField<_Offset, reg::BitMask32<6, 9>>{ };
			constexpr auto PLLM = reg::RWField<_Offset, reg::BitMask32<0, 6>>{ };
		};

		namespace CFGR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0008>>;
			constexpr auto MCO2 = reg::RWField<_Offset, reg::BitMask32<30, 2>>{ };
			constexpr auto MCO2PRE = reg::RWField<_Offset, reg::BitMask32<27, 3>>{ };
			constexpr auto MCO1PRE = reg::RWField<_Offset, reg::BitMask32<24, 3>>{ };

			enum class I2sSrcVal : std::uint32_t
			{
				PLL = 0x0,
				EXTERNAL = 0x1
			};

			constexpr auto I2SSRC = reg::RWField<_Offset, reg::BitMask32<23, 1>, I2sSrcVal>{ };
			constexpr auto MCO1 = reg::RWField<_Offset, reg::BitMask32<21, 2>>{ };
			constexpr auto RTCPRE = reg::RWField<_Offset, reg::BitMask32<16, 5>>{ };

			enum class PpreVal : std::uint32_t
			{
				NO_DIV = 0x00,
				DIV2 = 0x04,
				DIV4 = 0x05,
				DIV8 = 0x06,
				DIV16 = 0x07,
			};

			constexpr auto PPRE2 = reg::RWField<_Offset, reg::BitMask32<13, 3>, PpreVal>{ };
			constexpr auto PPRE1 = reg::RWField<_Offset, reg::BitMask32<10, 3>, PpreVal>{ };

			enum class HpreVal : std::uint32_t
			{
				NO_DIV = 0x0,
				DIV2   = 0x8,
				DIV4   = 0x9,
				DIV8   = 0xA,
				DIV16  = 0xB,
				DIV64  = 0xC,
				DIV128 = 0xD,
				DIV256 = 0xE,
				DIV512 = 0xF
			};

			constexpr auto HPRE = reg::RWField<_Offset, reg::BitMask32<4, 4>, HpreVal>{ };

			enum class SwVal : std::uint32_t
			{
				HSI = 0x0,
				HSE = 0x1,
				PLL = 0x2
			};

			constexpr auto SWS = reg::RWField<_Offset, reg::BitMask32<2, 2>, SwVal>{ };
			constexpr auto SW = reg::RWField<_Offset, reg::BitMask32<0, 2>, SwVal>{ };
		};

		namespace CIR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x000c>>;
			constexpr auto CSSC = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			constexpr auto PLLI2SRDYC = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto PLLRDYC = reg::RWField<_Offset, reg::BitMask32<20, 1>>{ };
			constexpr auto HSERDYC = reg::RWField<_Offset, reg::BitMask32<19, 1>>{ };
			constexpr auto HSIRDYC = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto LSERDYC = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto LSIRDYC = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto PLLI2SRDYIE = reg::RWField<_Offset, reg::BitMask32<13, 1>>{ };
			constexpr auto PLLRDYIE = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto HSERDYIE = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto HSIRDYIE = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			constexpr auto LSERDYIE = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			constexpr auto LSIRDYIE = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto CSSF = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto PLLI2SRDYF = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto PLLRDYF = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto HSERDYF = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto HSIRDYF = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto LSERDYF = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto LSIRDYF = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB1RSTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0010>>;
			constexpr auto OTGHSRST = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto ETHMACRST = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto DMA2RST = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto DMA1RST = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto CRCRST = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto GPIOIRST = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto GPIOHRST = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto GPIOGRST = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto GPIOFRST = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto GPIOERST = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto GPIODRST = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto GPIOCRST = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto GPIOBRST = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto GPIOARST = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB2RSTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0014>>;
			constexpr auto OTGFSRST = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto RNGRST = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto DCMIRST = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB3RSTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0018>>;
			constexpr auto FSMCRST = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB1RSTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0020>>;
			constexpr auto DACRST = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto PWRRST = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto CAN2RST = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto CAN1RST = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto I2C3RST = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			constexpr auto I2C2RST = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto I2C1RST = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto UART5RST = reg::RWField<_Offset, reg::BitMask32<20, 1>>{ };
			constexpr auto UART4RST = reg::RWField<_Offset, reg::BitMask32<19, 1>>{ };
			constexpr auto UART3RST = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto UART2RST = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto SPI3RST = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			constexpr auto SPI2RST = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto WWDGRST = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto TIM14RST = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto TIM13RST = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto TIM12RST = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto TIM7RST = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto TIM6RST = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM5RST = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto TIM4RST = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto TIM3RST = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM2RST = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB2RSTR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0024>>;
			constexpr auto TIM11RST = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto TIM10RST = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto TIM9RST = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto SYSCFGRST = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto SPI1RST = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto SDIORST = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto ADCRST = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto USART6RST = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto USART1RST = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM8RST = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM1RST = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB1ENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0030>>;
			constexpr auto OTGHSULPIEN = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };
			constexpr auto OTGHSEN = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto ETHMACPTPEN = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto ETHMACRXEN = reg::RWField<_Offset, reg::BitMask32<27, 1>>{ };
			constexpr auto ETHMACTXEN = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto ETHMACEN = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto DMA2EN = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto DMA1EN = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto BKPSRAMEN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto CRCEN = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto GPIOIEN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto GPIOHEN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto GPIOGEN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto GPIOFEN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto GPIOEEN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto GPIODEN = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto GPIOCEN = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto GPIOBEN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto GPIOAEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB2ENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0034>>;
			constexpr auto OTGFSEN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto RNGEN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto DCMIEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB3ENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0038>>;
			constexpr auto FSMCEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB1ENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0040>>;
			constexpr auto DACEN = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto PWREN = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto CAN2EN = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto CAN1EN = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto I2C3EN = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			constexpr auto I2C2EN = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto I2C1EN = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto UART5EN = reg::RWField<_Offset, reg::BitMask32<20, 1>>{ };
			constexpr auto UART4EN = reg::RWField<_Offset, reg::BitMask32<19, 1>>{ };
			constexpr auto USART3EN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto USART2EN = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto SPI3EN = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			constexpr auto SPI2EN = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto WWDGEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto TIM14EN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto TIM13EN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto TIM12EN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto TIM7EN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto TIM6EN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM5EN = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto TIM4EN = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto TIM3EN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM2EN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB2ENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0044>>;
			constexpr auto TIM11EN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto TIM10EN = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto TIM9EN = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto SYSCFGEN = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto SPI1EN = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto SDIOEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto ADC3EN = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			constexpr auto ADC2EN = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			constexpr auto ADC1EN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto USART6EN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto USART1EN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM8EN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM1EN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB1LPENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0050>>;
			constexpr auto OTGHSULPILPEN = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };
			constexpr auto OTGHSLPEN = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto ETHMACPTPLPEN = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto ETHMACRXLPEN = reg::RWField<_Offset, reg::BitMask32<27, 1>>{ };
			constexpr auto ETHMACTXLPEN = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto ETHMACLPEN = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto DMA2LPEN = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto DMA1LPEN = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto BKPSRAMLPEN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto SRAM2LPEN = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto SRAM1LPEN = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto FLITFLPEN = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			constexpr auto CRCLPEN = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto GPIOILPEN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto GPIOHLPEN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto GPIOGLPEN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto GPIOFLPEN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto GPIOELPEN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto GPIODLPEN = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto GPIOCLPEN = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto GPIOBLPEN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto GPIOALPEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB2LPENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0054>>;
			constexpr auto OTGFSLPEN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto RNGLPEN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto DCMILPEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace AHB3LPENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0058>>;
			constexpr auto FSMCLPEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB1LPENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0060>>;
			constexpr auto DACLPEN = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto PWRLPEN = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto CAN2LPEN = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto CAN1LPEN = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto I2C3LPEN = reg::RWField<_Offset, reg::BitMask32<23, 1>>{ };
			constexpr auto I2C2LPEN = reg::RWField<_Offset, reg::BitMask32<22, 1>>{ };
			constexpr auto I2C1LPEN = reg::RWField<_Offset, reg::BitMask32<21, 1>>{ };
			constexpr auto UART5LPEN = reg::RWField<_Offset, reg::BitMask32<20, 1>>{ };
			constexpr auto UART4LPEN = reg::RWField<_Offset, reg::BitMask32<19, 1>>{ };
			constexpr auto USART3LPEN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto USART2LPEN = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto SPI3LPEN = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			constexpr auto SPI2LPEN = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto WWDGLPEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto TIM14LPEN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto TIM13LPEN = reg::RWField<_Offset, reg::BitMask32<7, 1>>{ };
			constexpr auto TIM12LPEN = reg::RWField<_Offset, reg::BitMask32<6, 1>>{ };
			constexpr auto TIM7LPEN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto TIM6LPEN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM5LPEN = reg::RWField<_Offset, reg::BitMask32<3, 1>>{ };
			constexpr auto TIM4LPEN = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto TIM3LPEN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM2LPEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace APB2LPENR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0064>>;
			constexpr auto TIM11LPEN = reg::RWField<_Offset, reg::BitMask32<18, 1>>{ };
			constexpr auto TIM10LPEN = reg::RWField<_Offset, reg::BitMask32<17, 1>>{ };
			constexpr auto TIM9LPEN = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto SYSCFGLPEN = reg::RWField<_Offset, reg::BitMask32<14, 1>>{ };
			constexpr auto SPI1LPEN = reg::RWField<_Offset, reg::BitMask32<12, 1>>{ };
			constexpr auto SDIOLPEN = reg::RWField<_Offset, reg::BitMask32<11, 1>>{ };
			constexpr auto ADC3LPEN = reg::RWField<_Offset, reg::BitMask32<10, 1>>{ };
			constexpr auto ADC2LPEN = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			constexpr auto ADC1LPEN = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto USART6LPEN = reg::RWField<_Offset, reg::BitMask32<5, 1>>{ };
			constexpr auto USART1LPEN = reg::RWField<_Offset, reg::BitMask32<4, 1>>{ };
			constexpr auto TIM8LPEN = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto TIM1LPEN = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace BDCR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0070>>;
			constexpr auto BDRST = reg::RWField<_Offset, reg::BitMask32<16, 1>>{ };
			constexpr auto RTCEN = reg::RWField<_Offset, reg::BitMask32<15, 1>>{ };
			constexpr auto RTCSEL1 = reg::RWField<_Offset, reg::BitMask32<9, 1>>{ };
			constexpr auto RTCSEL0 = reg::RWField<_Offset, reg::BitMask32<8, 1>>{ };
			constexpr auto LSEBYP = reg::RWField<_Offset, reg::BitMask32<2, 1>>{ };
			constexpr auto LSERDY = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto LSEON = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace CSR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0074>>;
			constexpr auto LPWRRSTF = reg::RWField<_Offset, reg::BitMask32<31, 1>>{ };
			constexpr auto WWDGRSTF = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };
			constexpr auto WDGRSTF = reg::RWField<_Offset, reg::BitMask32<29, 1>>{ };
			constexpr auto SFTRSTF = reg::RWField<_Offset, reg::BitMask32<28, 1>>{ };
			constexpr auto PORRSTF = reg::RWField<_Offset, reg::BitMask32<27, 1>>{ };
			constexpr auto PADRSTF = reg::RWField<_Offset, reg::BitMask32<26, 1>>{ };
			constexpr auto BORRSTF = reg::RWField<_Offset, reg::BitMask32<25, 1>>{ };
			constexpr auto RMVF = reg::RWField<_Offset, reg::BitMask32<24, 1>>{ };
			constexpr auto LSIRDY = reg::RWField<_Offset, reg::BitMask32<1, 1>>{ };
			constexpr auto LSION = reg::RWField<_Offset, reg::BitMask32<0, 1>>{ };
		};

		namespace SSCGR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0080>>;
			constexpr auto SSCGEN = reg::RWField<_Offset, reg::BitMask32<31, 1>>{ };
			constexpr auto SPREADSEL = reg::RWField<_Offset, reg::BitMask32<30, 1>>{ };
			constexpr auto INCSTEP = reg::RWField<_Offset, reg::BitMask32<13, 15>>{ };
			constexpr auto MODPER = reg::RWField<_Offset, reg::BitMask32<0, 13>>{ };
		};

		namespace PLLI2SCFGR
		{
			using _Offset = reg::FieldLocation<std::uint32_t, tag, reg::FieldOffset<std::uint32_t, 0x0084>>;
			constexpr auto PLLI2SRx = reg::RWField<_Offset, reg::BitMask32<28, 3>>{ };
			constexpr auto PLLI2SNx = reg::RWField<_Offset, reg::BitMask32<6, 9>>{ };
		};
	};
}
