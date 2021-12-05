#pragma once
#include "reg/field.hpp"

namespace drivers::cs43l22::regmap
{
	struct tag { };
	namespace ID
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x01>>;
		constexpr auto REVID = reg::ROField<_Offset, reg::BitMask8<0, 3>>{ };
		constexpr auto CHIPID = reg::ROField<_Offset, reg::BitMask8<3, 5>>{ };
	}
	namespace PowerCtl1
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x02>>;

		enum class PdnVal : std::uint8_t
		{
			OFF = 0x01,
			ON = 0x9e
		};

		// Configures the power state of the CS43L22.
		constexpr auto PDN = reg::RWField<_Offset, reg::BitMask8<0, 8>, PdnVal>{ };
	}
	namespace PowerCtl2
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x04>>;
		// Speaker A Power Control
		constexpr auto PDN_SPKA = reg::RWField<_Offset, reg::BitMask8<0, 2>>{ };
		// Speaker B Power Control
		constexpr auto PDN_SPKB = reg::RWField<_Offset, reg::BitMask8<2, 2>>{ };
		// Headphone A Power Control
		constexpr auto PDN_HPA = reg::RWField<_Offset, reg::BitMask8<4, 2>>{ };
		// Headphone B Power Control
		constexpr auto PDN_HPB = reg::RWField<_Offset, reg::BitMask8<6, 2>>{ };
	}
	namespace ClockingCtl
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x05>>;
		constexpr auto MCLKDIV2 = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto RATIO = reg::RWField<_Offset, reg::BitMask8<1, 2>>{ };
		constexpr auto VIDEOCLK = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto K32GROUP = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto SPEED = reg::RWField<_Offset, reg::BitMask8<5, 2>>{ };
		constexpr auto AUTO = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace InterfaceCtl1
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x06>>;
		constexpr auto WORD_LENGTH = reg::RWField<_Offset, reg::BitMask8<0, 2>>{ };

		enum class InterfaceFormatVal : std::uint8_t
		{
			LEFT_JUSTIFIED = 0,
			I2S = 1,
			RIGHT_JUSTIFIED = 2
		};

		constexpr auto INTERFACE_FORMAT = reg::RWField<_Offset, reg::BitMask8<2, 2>, InterfaceFormatVal>{ };
		constexpr auto DSP_MODE = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto SCKL_POL = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
		constexpr auto MASTER_SLAVE = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace InterfaceCtl2
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x07>>;
		constexpr auto INV_SWCH = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto SCLK_IS_MCLK = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
	}
	namespace PassthroughASelect
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x08>>;
		constexpr auto PASSASEL = reg::RWField<_Offset, reg::BitMask8<0, 4>>{ };
	}
	namespace PassthroughBSelect
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x09>>;
		constexpr auto PASSBSEL = reg::RWField<_Offset, reg::BitMask8<0, 4>>{ };
	}
	namespace AnalogZCandSRSettings
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x0a>>;
		constexpr auto ANLGZCA = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto ANLGSFTA = reg::RWField<_Offset, reg::BitMask8<1, 1>>{ };
		constexpr auto ANLGZCB = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
		constexpr auto ANLGSFTB = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
	}
	namespace PassthroughGangControl
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x0c>>;
		constexpr auto PASSB_IS_A = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace PlaybackCtl1
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x0d>>;
		constexpr auto MSTAMUTE = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto MSTBMUTE = reg::RWField<_Offset, reg::BitMask8<1, 1>>{ };
		constexpr auto INV_PCM = reg::RWField<_Offset, reg::BitMask8<2, 2>>{ };
		constexpr auto PLYBCKB_IS_A = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto HPGAIN = reg::RWField<_Offset, reg::BitMask8<5, 3>>{ };
	}
	namespace MiscCtl
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x0e>>;
		constexpr auto DIGZC = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto DIGSFT = reg::RWField<_Offset, reg::BitMask8<1, 1>>{ };
		constexpr auto DEEMPH = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
		constexpr auto FREEZE = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto PASSAMUTE = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto PASSBMUTE = reg::RWField<_Offset, reg::BitMask8<5, 1>>{ };
		constexpr auto PASSTHRU = reg::RWField<_Offset, reg::BitMask8<6, 2>>{ };
	}
	namespace PlaybackCtl2
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x0f>>;
		constexpr auto MUTE50_50 = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto SPKMONO = reg::RWField<_Offset, reg::BitMask8<1, 1>>{ };
		constexpr auto SPKSWAP = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
		constexpr auto SPKB_IS_A = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto SPKAMUTE = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto SPKBMUTE = reg::RWField<_Offset, reg::BitMask8<5, 1>>{ };
		constexpr auto HPAMUTE = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
		constexpr auto HPBMUTE = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace PassthroughAVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x14>>;
		constexpr auto PASSAVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace PassthroughBVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x15>>;
		constexpr auto PASSBVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace PCMAVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1a>>;
		constexpr auto PCMAVOL = reg::RWField<_Offset, reg::BitMask8<0, 7>>{ };
		constexpr auto PCMAMUTE = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace PCMBVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1b>>;
		constexpr auto PCMBVOL = reg::RWField<_Offset, reg::BitMask8<0, 7>>{ };
		constexpr auto PCMBMUTE = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace BEEPFreqOnTime
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1c>>;
		constexpr auto ONTIME = reg::RWField<_Offset, reg::BitMask8<0, 4>>{ };
		constexpr auto FREQ = reg::RWField<_Offset, reg::BitMask8<4, 4>>{ };
	}
	namespace BEEPVolOffTime
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1d>>;
		constexpr auto BPVOL = reg::RWField<_Offset, reg::BitMask8<0, 5>>{ };
		constexpr auto OFFTIME = reg::RWField<_Offset, reg::BitMask8<5, 3>>{ };
	}
	namespace BEEPToneCfg
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1e>>;
		constexpr auto TC_EN = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto BASS_CF = reg::RWField<_Offset, reg::BitMask8<1, 2>>{ };
		constexpr auto TREB_CF = reg::RWField<_Offset, reg::BitMask8<3, 2>>{ };
		constexpr auto BEEPMIXDIS = reg::RWField<_Offset, reg::BitMask8<5, 1>>{ };
		constexpr auto BEEP = reg::RWField<_Offset, reg::BitMask8<6, 2>>{ };
	}
	namespace ToneCtl
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x1f>>;
		constexpr auto BASS = reg::RWField<_Offset, reg::BitMask8<0, 4>>{ };
		constexpr auto TREB = reg::RWField<_Offset, reg::BitMask8<4, 4>>{ };
	}
	namespace MasterAVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x20>>;
		constexpr auto MSTAVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace MasterBVol
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x21>>;
		constexpr auto MSTBVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace HeadphoneAVolume
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x22>>;
		constexpr auto HPAVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace HeadphoneBVolume
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x23>>;
		constexpr auto HPBVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace SpeakerAVolume
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x24>>;
		constexpr auto SPKAVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace SpeakerBVolume
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x25>>;
		constexpr auto SPKBVOL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace ChannelMixerAndSwap
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x26>>;
		constexpr auto PCMBSWP = reg::RWField<_Offset, reg::BitMask8<4, 2>>{ };
		constexpr auto PCMASWP = reg::RWField<_Offset, reg::BitMask8<6, 2>>{ };
	}
	namespace LimitCtl1
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x27>>;
		constexpr auto LIMZCDIS = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
		constexpr auto LIMSRDIS = reg::RWField<_Offset, reg::BitMask8<1, 1>>{ };
		constexpr auto CUSH = reg::RWField<_Offset, reg::BitMask8<2, 3>>{ };
		constexpr auto LMAX = reg::RWField<_Offset, reg::BitMask8<5, 3>>{ };
	}
	namespace LimitCtl2
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x28>>;
		constexpr auto LIMRRATE = reg::RWField<_Offset, reg::BitMask8<0, 6>>{ };
		constexpr auto LIMIT_ALL = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
		constexpr auto LIMIT = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace LimiterAttackRate
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x29>>;
		constexpr auto LIMARATE = reg::RWField<_Offset, reg::BitMask8<0, 6>>{ };
	}
	namespace OverflowAndClockStatus
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x2e>>;
		constexpr auto PCMBOVFL = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
		constexpr auto PCMAOVFL = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto DSPAOVFL = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto DSPBOVFL = reg::RWField<_Offset, reg::BitMask8<5, 1>>{ };
		constexpr auto SPCLKERR = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
	}
	namespace BatteryCompensation
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x2f>>;
		constexpr auto VPREF = reg::RWField<_Offset, reg::BitMask8<0, 4>>{ };
		constexpr auto VPMONITOR = reg::RWField<_Offset, reg::BitMask8<6, 1>>{ };
		constexpr auto BATTCMP = reg::RWField<_Offset, reg::BitMask8<7, 1>>{ };
	}
	namespace VPBatteryLevel
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x30>>;
		constexpr auto VPLVL = reg::RWField<_Offset, reg::BitMask8<0, 8>>{ };
	}
	namespace SpeakerStatus
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x31>>;
		constexpr auto SPKR_HP = reg::RWField<_Offset, reg::BitMask8<3, 1>>{ };
		constexpr auto SPKBSHRT = reg::RWField<_Offset, reg::BitMask8<4, 1>>{ };
		constexpr auto SPKASHRT = reg::RWField<_Offset, reg::BitMask8<5, 1>>{ };
	}
	namespace ChargePumpFrequency
	{
		using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint8_t, 0x34>>;
		constexpr auto CHGFREQ = reg::RWField<_Offset, reg::BitMask8<4, 4>>{ };
	}
}