#include "register.hpp"

namespace drivers
{

namespace vl6180
{
    namespace regmap
    {
        struct tag { };

        namespace MODEL_ID 
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0000>>;
            constexpr auto MODEL_ID = reg::ROField<_Offset, reg::BitMask8<0, 8>>{ };
        }
        
        namespace MODEL_REV
        {
            using _Offset = reg::FieldLocation<std::uint16_t, tag, reg::FieldOffset<std::uint16_t, 0x0001>>;
            constexpr auto MAJOR = reg::ROField<_Offset, reg::BitMask16<0, 3>>{ };
            constexpr auto MINOR = reg::ROField<_Offset, reg::BitMask16<8, 3>>{ };
        }
        
        namespace MODULE_REV
        {
            using _Offset = reg::FieldLocation<std::uint16_t, tag, reg::FieldOffset<std::uint16_t, 0x0003>>;
            constexpr auto MAJOR = reg::ROField<_Offset, reg::BitMask16<0, 3>>{ };
            constexpr auto MINOR = reg::ROField<_Offset, reg::BitMask16<8, 3>>{ };
        }

        /*namespace IDENTIFICATION__DATE_HI
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0006>>;
            IDENTIFICATION__DATE_HI
        }
        
        namespace IDENTIFICATION__DATE_LO
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0007>>;
            IDENTIFICATION__DATE_LO
        }

        0x008:0x009 IDENTIFICATION__TIME Section 6.2.8 on page 44 */

        namespace MODE_GPIO0
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0010>>;
            
            enum class ModeVal : std::uint8_t
            {
                OFF = 0x0, 
                INTERRUPT_OUTPUT = 0x8
            };
            constexpr auto MODE = reg::RWField<_Offset, reg::BitMask8<1, 4>, ModeVal>{ };

            enum class PolarityVal
            {
                ACTIVE_LOW = 0,
                ACTIVE_HIGH = 1
            };
            constexpr auto POLARITY = reg::RWField<_Offset, reg::BitMask8<5, 1>, PolarityVal>{ };
        }

        namespace MODE_GPIO1
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0011>>;
            enum class PolarityVal
            {
                ACTIVE_LOW = 0,
                ACTIVE_HIGH = 1
            };
            constexpr auto POLARITY = reg::RWField<_Offset, reg::BitMask8<5, 1>, PolarityVal>{ };

            enum class ModeVal : std::uint8_t
            {
                OFF = 0x0, 
                INTERRUPT_OUTPUT = 0x8
            };
            constexpr auto MODE = reg::RWField<_Offset, reg::BitMask8<1, 4>, ModeVal>{ };
        }

        namespace HISTORY_CTRL
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0012>>;
            constexpr auto ENABLE = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
            constexpr auto CLEAR = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
        }

        namespace INTERRUPT_CONFIG_GPIO
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0014>>;

            enum class ModeVal : std::uint8_t
            {
                DISABLED = 0x0,
                LEVEL_LOW = 0x1,
                LEVEL_HIGH = 0x2,
                OUT_OF_WINDOW = 0x3,
                NEW_SAMPLE_READY = 0x4
            };
            constexpr auto MODE = reg::RWField<_Offset, reg::BitMask8<0, 3>, ModeVal>{ };
        }

        namespace INTERRUPT_CLEAR
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0015>>;
            constexpr auto RANGE_INTERRUPT = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
            constexpr auto ERROR_INTERRUPT = reg::RWField<_Offset, reg::BitMask8<2, 1>>{ };
        }

        namespace FRESH_OUT_OF_RESET
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0016>>;
            constexpr auto FRESH_OUT_OF_RESET = reg::RWField<_Offset, reg::BitMask8<0, 1>>{ };
        }

        /*namespace GROUPED_PARAMETER_HOLD
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0017>>;
            
        }*/

        namespace SYSRANGE_CONTROL
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0018>>;
            
            /* StartStop trigger based on current mode and system configuration of
            device_ready. FW clears register automatically.
            Setting this bit to 1 in single-shot mode starts a single measurement. 
            Setting this bit to 1 in continuous mode will either start continuous operation (if stopped) or halt
            continuous operation (if started). 
            This bit is auto-cleared in both modes of operation.*/
            constexpr auto START_STOP = reg::WOField<_Offset, reg::BitMask8<0, 1>>{};

            enum class ModeVal : std::uint8_t
            {
                SINGLE_SHOT = 0, 
                CONTINUOUS = 1
            };
            constexpr auto MODE = reg::WOField<_Offset, reg::BitMask8<1, 1>, ModeVal>{};
        }

        namespace SYSRANGE_THRESH_HIGH
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0019>>;
        }

        namespace SYSRANGE_THRESH_LOW
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x001A>>;
        }

        namespace SYSRANGE_INTERMEASUREMENT_PERIOD
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x001B>>;
        }

        namespace SYSRANGE_MAX_CONVERGENCE_TIME
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x001C>>;
        }

        namespace SYSRANGE_CROSSTALK_COMPENSATION_RATE
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x001E>>;
        }

        namespace SYSRANGE__CROSSTALK_VALID_HEIGHT
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0021>>;
        }

        namespace SYSRANGE__EARLY_CONVERGENCE_ESTIMATE
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0022>>;
        }

        namespace SYSRANGE__PART_TO_PART_RANGE_OFFSET
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0024>>;
        }

        namespace SYSRANGE__RANGE_IGNORE_VALID_HEIGHT
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0025>>;
        }

        namespace SYSRANGE__RANGE_IGNORE_THRESHOLD
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0026>>;
        }

        namespace SYSRANGE__MAX_AMBIENT_LEVEL_MULT
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x002C>>;
        }

        namespace SYSRANGE__RANGE_CHECK_ENABLES
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x002D>>;
        }

        namespace SYSRANGE__VHV_RECALIBRATE
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x002E>>;
        }

        namespace SYSRANGE__VHV_REPEAT_RATE
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0031>>;
        }

        namespace RANGE_STATUS
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x004D>>;

            enum class ErrorCodeVal : std::uint8_t
            {
                NO_ERROR = 0x0,
                VCSEL_CONTINUITY_TEST = 0x1,
                VCSEL_WATCHDOG_TEST = 0x2,
                VCSEL_WATCHDOG = 0x3,
                PLL1_LOCK = 0x4,
                PLL2_LOCK = 0x5,
                EARLY_CONVERGENCE_ESTIMATE = 0x6,
                MAX_CONVERGENCE = 0x7,
                NO_TARGET_IGNORE = 0x8,
                MAX_SIGNAL_TO_NOISE_RATIO = 0xB,
                RAW_RANGING_ALGO_UNDERFLOW = 0xC,
                RAW_RANGING_ALGO_OVERFLOW = 0xD,
                RANGING_ALGO_UNDERFLOW = 0xE,
                RANGING_ALGO_OVERFLOW = 0xF
            };
            constexpr auto ERROR_CODE = reg::ROField<_Offset, reg::BitMask8<4, 4>, ErrorCodeVal>{};
            constexpr auto DEVICE_READY = reg::ROField<_Offset, reg::BitMask8<0, 1>>{};
        }

        namespace INTERRUPT_STATUS
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x004F>>;

            enum class ErrorInterruptVal : std::uint8_t
            {
                NO_ERROR = 0,
                LASER_SAFETY_ERROR = 1,
                PLL_ERROR = 2
            };
            constexpr auto ERROR_INTERRUPT = reg::ROField<_Offset, reg::BitMask8<6, 1>, ErrorInterruptVal>{};

            enum class RangeInterruptVal : std::uint8_t
            {
                NO_THRESHOLD_EVENT = 0,
                LEVEL_LOW_THRESHOLD = 1,
                LEVEL_HIGH_THRESHOLD = 2,
                OUT_OF_WINDOW_THRESHOLD = 3,
                NEW_SAMPLE_READY = 4
            };
            constexpr auto RANGE_INTERRUPT = reg::ROField<_Offset, reg::BitMask8<0, 3>, RangeInterruptVal>{};
        }

        /*0x052:0x060 (0x2) RESULT__HISTORY_BUFFER_x Section 6.2.33 on page 57*/

        namespace RANGE_VAL
        {
            using _Offset = reg::FieldLocation<std::uint8_t, tag, reg::FieldOffset<std::uint16_t, 0x0062>>;
        }
    }
}

}