#pragma once
#include "i2c/i2c_like.hpp"
#include "i2c/i2c_memory.hpp"

#include "reg/unchecked_write.hpp"
#include "reg/unchecked_read.hpp"
#include "reg/apply.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/bit_is_set.hpp"

#include "vl6180/regmap.hpp"

#include "async/sequence.hpp"
#include "async/then.hpp"
#include "async/conditional.hpp"
#include "async/repeat.hpp"
#include "async/just.hpp"

namespace drivers
{
    namespace vl6180
    {
        struct DeviceIdentification
        {
            struct Revision
            {
                std::uint8_t major;
                std::uint8_t minor;
            };
            std::uint8_t modelId;
            Revision modelRevision;
            Revision moduleRevision;
        };
    }

    template<i2c::I2cLike I2cDevice>
    class Vl6180
    {
        template<std::uint16_t addr>
        using Loc = reg::FieldLocation<std::uint8_t, vl6180::regmap::tag, reg::FieldOffset<std::uint16_t, addr>>;

        struct AddressSerializer
        {
            static void serializeAddress(std::uint16_t t, std::uint8_t * buf)
            {
                buf[0] = (t >> 8) & 0xff;
                buf[1] = t & 0xff;
            }
        };

        using I2cMemoryType = i2c::I2cMemory<I2cDevice, vl6180::regmap::tag, AddressSerializer>;

    public:
        Vl6180(I2cDevice & i2cDevice) : device_(i2cDevice, 0x29) { }

        Vl6180(const Vl6180 &) = delete;
        Vl6180 & operator=(const Vl6180 &) = delete;

        auto init()
        {
            using namespace vl6180::regmap;

            return 
                reg::bitIsSet(device_, FRESH_OUT_OF_RESET::FRESH_OUT_OF_RESET)
                | async::then([this](bool isFreshOutOfReset) {
                    return async::conditional(isFreshOutOfReset, setupRegisterDefaults(), async::justValue());
                })
                | async::then([this]() {
                    return reg::apply(device_,
                            reg::set(INTERRUPT_CLEAR::RANGE_INTERRUPT),
                            reg::set(INTERRUPT_CLEAR::ERROR_INTERRUPT));
                });
        }

        async::Sender auto getDeviceIdentification()
        {
            using namespace vl6180::regmap;
            return async::useState(
                vl6180::DeviceIdentification{},
                [this](auto & deviceIdentification) {
                    return async::sequence(
                        reg::read(device_, MODEL_ID::MODEL_ID)
                        | async::transform([&](auto modelId) {
                            deviceIdentification.modelId = modelId;
                        }),

                        reg::apply(device_, 
                            reg::read(MODEL_REV::MINOR),
                            reg::read(MODEL_REV::MAJOR))
                        | async::transform([&](auto modelMinorVersion, auto modelMajorVersion) {
                            deviceIdentification.modelRevision.minor = modelMinorVersion;
                            deviceIdentification.modelRevision.major = modelMajorVersion;
                        }),

                        reg::apply(device_, 
                            reg::read(MODULE_REV::MINOR),
                            reg::read(MODULE_REV::MAJOR))
                        | async::transform([&](auto moduleMinorVersion, auto moduleMajorVersion) {
                            deviceIdentification.moduleRevision.minor = moduleMinorVersion;
                            deviceIdentification.moduleRevision.major = moduleMajorVersion;
                        })
                    ) 
                    | async::transform([&]() {
                        return deviceIdentification;
                    });
                });
        }

        auto readRange()
        {
            using namespace vl6180::regmap;
            return async::sequence(
                startMeasurement(),
                waitForSampleReady(),
                reg::uncheckedRead(device_, RANGE_VAL::_Offset{})
                | async::then([this](auto rangeValue) {
                    return 
                        // Clear interrupt flags
                        reg::apply(device_,
                            reg::set(INTERRUPT_CLEAR::RANGE_INTERRUPT),
                            reg::set(INTERRUPT_CLEAR::ERROR_INTERRUPT))
                        | async::transform([rangeValue]() { return rangeValue; });
                }));
        }
        
    private:
        auto startMeasurement()
        {
            using namespace vl6180::regmap;
            return reg::apply(device_,
                reg::set(SYSRANGE_CONTROL::START_STOP),
                reg::write(SYSRANGE_CONTROL::MODE, constant_c<SYSRANGE_CONTROL::ModeVal::SINGLE_SHOT>));
        }

        auto waitForSampleReady()
        {
            using namespace vl6180::regmap;
            return async::repeatUntil(
                reg::read(device_, INTERRUPT_STATUS::RANGE_INTERRUPT),
                [](auto value) { 
                    return value == 0x4;//INTERRUPT_STATUS::RangeInterruptVal::NEW_SAMPLE_READY; 
                });
        }

        auto setupRegisterDefaults()
        {
            using namespace vl6180::regmap;
            return async::sequence(
                // Private (undocumented) registers
                reg::uncheckedWrite(device_, Loc<0x0207>{}, uint8_c<0x01>),
                reg::uncheckedWrite(device_, Loc<0x0208>{}, uint8_c<0x01>),
                reg::uncheckedWrite(device_, Loc<0x0096>{}, uint8_c<0x00>),
                reg::uncheckedWrite(device_, Loc<0x0097>{}, uint8_c<0xfd>),
                reg::uncheckedWrite(device_, Loc<0x00e3>{}, uint8_c<0x00>),
                reg::uncheckedWrite(device_, Loc<0x00e4>{}, uint8_c<0x04>),
                reg::uncheckedWrite(device_, Loc<0x00e5>{}, uint8_c<0x02>),
                reg::uncheckedWrite(device_, Loc<0x00e6>{}, uint8_c<0x01>),
                reg::uncheckedWrite(device_, Loc<0x00e7>{}, uint8_c<0x03>),
                reg::uncheckedWrite(device_, Loc<0x00f5>{}, uint8_c<0x02>),
                reg::uncheckedWrite(device_, Loc<0x00d9>{}, uint8_c<0x05>),
                reg::uncheckedWrite(device_, Loc<0x00db>{}, uint8_c<0xce>),
                reg::uncheckedWrite(device_, Loc<0x00dc>{}, uint8_c<0x03>),
                reg::uncheckedWrite(device_, Loc<0x00dd>{}, uint8_c<0xf8>),
                reg::uncheckedWrite(device_, Loc<0x009f>{}, uint8_c<0x00>),
                reg::uncheckedWrite(device_, Loc<0x00a3>{}, uint8_c<0x3c>),
                reg::uncheckedWrite(device_, Loc<0x00b7>{}, uint8_c<0x00>),
                reg::uncheckedWrite(device_, Loc<0x00bb>{}, uint8_c<0x3c>),
                reg::uncheckedWrite(device_, Loc<0x00b2>{}, uint8_c<0x09>),
                reg::uncheckedWrite(device_, Loc<0x00ca>{}, uint8_c<0x09>),
                reg::uncheckedWrite(device_, Loc<0x0198>{}, uint8_c<0x01>),
                reg::uncheckedWrite(device_, Loc<0x01b0>{}, uint8_c<0x17>),
                reg::uncheckedWrite(device_, Loc<0x01ad>{}, uint8_c<0x00>),
                reg::uncheckedWrite(device_, Loc<0x00ff>{}, uint8_c<0x05>),
                reg::uncheckedWrite(device_, Loc<0x0100>{}, uint8_c<0x05>),
                reg::uncheckedWrite(device_, Loc<0x0199>{}, uint8_c<0x05>),
                reg::uncheckedWrite(device_, Loc<0x01a6>{}, uint8_c<0x1b>),
                reg::uncheckedWrite(device_, Loc<0x01ac>{}, uint8_c<0x3e>),
                reg::uncheckedWrite(device_, Loc<0x01a7>{}, uint8_c<0x1f>),
                reg::uncheckedWrite(device_, Loc<0x0030>{}, uint8_c<0x00>),
                // Enable polling for ‘New Sample ready’ when measurement completes
                reg::uncheckedWrite(device_, Loc<0x0011>{}, uint8_c<0x10>),
                // Set the averaging sample period (compromise between lower noise and increased execution time)
                reg::uncheckedWrite(device_, Loc<0x010a>{}, uint8_c<0x30>), 
                // Set the light and dark gain (upper nibble). Dark gain should not be changed.
                reg::uncheckedWrite(device_, Loc<0x003f>{}, uint8_c<0x46>), 
                // Set the # of range measurements after which auto calibration of system is performed
                reg::uncheckedWrite(device_, Loc<0x0031>{}, uint8_c<0xFF>), 
                // Set ALS integration time to 100ms
                reg::uncheckedWrite(device_, Loc<0x0040>{}, uint8_c<0x63>), 
                // perform a single temperature calibration of the ranging sensor
                reg::uncheckedWrite(device_, Loc<0x002e>{}, uint8_c<0x01>), 

                // Set default ranging inter-measurement period to 100ms
                reg::uncheckedWrite(device_, Loc<0x001b>{}, uint8_c<0x09>),
                // Set default ALS inter-measurement period to 500ms
                reg::uncheckedWrite(device_, Loc<0x003e>{}, uint8_c<0x31>), 
                // Configures interrupt on ‘New Sample Ready threshold event’
                reg::uncheckedWrite(device_, Loc<0x0014>{}, uint8_c<0x24>),

                //reg::write(memory, INTERRUPT_CONFIG_GPIO::MODE, INTERRUPT_CONFIG_GPIO::ModeVal::NEW_SAMPLE_READY);

                reg::clear(device_, FRESH_OUT_OF_RESET::FRESH_OUT_OF_RESET));
        }

        I2cMemoryType device_;
    };
}