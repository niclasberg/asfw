#pragma once
#include "i2c/i2c_like.hpp"
#include "reg/unchecked_write.hpp"
#include "reg/unchecked_read.hpp"
#include "reg/apply.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "async/sequence.hpp"
#include "async/then.hpp"
#include "async/conditional.hpp"

namespace drivers
{
    template<i2c::I2cLike I2cDevice>
    class Vl6180
    {
    public:
        explicit Vl6180(I2cDevice & i2cDevice) : i2cDevice_(i2cDevice) { }

        auto init()
        {
            return reg::bitIsSet(memory, FRESH_OUT_OF_RESET::FRESH_OUT_OF_RESET)
                | async::then([this](bool isFreshOutOfReset) {
                    return async::conditional(isFreshOutOfReset, setupRegisterDefaults(), async::justValue()),
                })
                | async::then([this]() {
                    return reg::apply(memory,
                            reg::set(regmap::INTERRUPT_CLEAR::RANGE_INTERRUPT),
                            reg::set(regmap::INTERRUPT_CLEAR::ERROR_INTERRUPT)));
                });
        }

        auto readRange()
        {
            return async::sequence(
                reg::apply(
                    reg::set(SYSRANGE_CONTROL::START_STOP),
                    reg::write(SYSRANGE_CONTROL::MODE, constant_c<SYSRANGE_CONTROL::ModeVal::SINGLE_SHOT>)),
                // TODO Poll for sample ready
                reg::uncheckedRead(regmap::RANGE_VAL::_Offset{}),
                // Clear interrupt flags
                reg::apply(
                    reg::set(regmap::INTERRUPT_CLEAR::RANGE_INTERRUPT),
                    reg::set(regmap::INTERRUPT_CLEAR::ERROR_INTERRUPT))
            );
        }
        
    private:
        auto setupRegisterDefaults()
        {
            return async::sequence(
                // Private (undocumented) registers
                reg::uncheckedWrite(Loc<0x0207>{}, uint8_c<0x01>),
                reg::uncheckedWrite(Loc<0x0208>{}, uint8_c<0x01>),
                reg::uncheckedWrite(Loc<0x0096>{}, uint8_c<0x00>),
                reg::uncheckedWrite(Loc<0x0097>{}, uint8_c<0xfd>),
                reg::uncheckedWrite(Loc<0x00e3>{}, uint8_c<0x00>),
                reg::uncheckedWrite(Loc<0x00e4>{}, uint8_c<0x04>),
                reg::uncheckedWrite(Loc<0x00e5>{}, uint8_c<0x02>),
                reg::uncheckedWrite(Loc<0x00e6>{}, uint8_c<0x01>),
                reg::uncheckedWrite(Loc<0x00e7>{}, uint8_c<0x03>),
                reg::uncheckedWrite(Loc<0x00f5>{}, uint8_c<0x02>),
                reg::uncheckedWrite(Loc<0x00d9>{}, uint8_c<0x05>),
                reg::uncheckedWrite(Loc<0x00db>{}, uint8_c<0xce>),
                reg::uncheckedWrite(Loc<0x00dc>{}, uint8_c<0x03>),
                reg::uncheckedWrite(Loc<0x00dd>{}, uint8_c<0xf8>),
                reg::uncheckedWrite(Loc<0x009f>{}, uint8_c<0x00>),
                reg::uncheckedWrite(Loc<0x00a3>{}, uint8_c<0x3c>),
                reg::uncheckedWrite(Loc<0x00b7>{}, uint8_c<0x00>),
                reg::uncheckedWrite(Loc<0x00bb>{}, uint8_c<0x3c>),
                reg::uncheckedWrite(Loc<0x00b2>{}, uint8_c<0x09>),
                reg::uncheckedWrite(Loc<0x00ca>{}, uint8_c<0x09>),
                reg::uncheckedWrite(Loc<0x0198>{}, uint8_c<0x01>),
                reg::uncheckedWrite(Loc<0x01b0>{}, uint8_c<0x17>),
                reg::uncheckedWrite(Loc<0x01ad>{}, uint8_c<0x00>),
                reg::uncheckedWrite(Loc<0x00ff>{}, uint8_c<0x05>),
                reg::uncheckedWrite(Loc<0x0100>{}, uint8_c<0x05>),
                reg::uncheckedWrite(Loc<0x0199>{}, uint8_c<0x05>),
                reg::uncheckedWrite(Loc<0x01a6>{}, uint8_c<0x1b>),
                reg::uncheckedWrite(Loc<0x01ac>{}, uint8_c<0x3e>),
                reg::uncheckedWrite(Loc<0x01a7>{}, uint8_c<0x1f>),
                reg::uncheckedWrite(Loc<0x0030>{}, uint8_c<0x00>),
                // Enable polling for ‘New Sample ready’ when measurement completes
                reg::uncheckedWrite(Loc<0x0011>{}, uint8_c<0x10>),
                // Set the averaging sample period (compromise between lower noise and increased execution time)
                reg::uncheckedWrite(Loc<0x010a>{}, uint8_c<0x30>), 
                // Set the light and dark gain (upper nibble). Dark gain should not be changed.
                reg::uncheckedWrite(Loc<0x003f>{}, uint8_c<0x46>), 
                // Set the # of range measurements after which auto calibration of system is performed
                reg::uncheckedWrite(Loc<0x0031>{}, uint8_c<0xFF>), 
                // Set ALS integration time to 100ms
                reg::uncheckedWrite(Loc<0x0040>{}, uint8_c<0x63>), 
                // perform a single temperature calibration of the ranging sensor
                reg::uncheckedWrite(Loc<0x002e>{}, uint8_c<0x01>), 

                // Set default ranging inter-measurement period to 100ms
                reg::uncheckedWrite(Loc<0x001b>{}, uint8_c<0x09>),
                // Set default ALS inter-measurement period to 500ms
                reg::uncheckedWrite(Loc<0x003e>{}, uint8_c<0x31>), 
                // Configures interrupt on ‘New Sample Ready threshold event’
                reg::uncheckedWrite(Loc<0x0014>{}, uint8_c<0x24>),

                //reg::write(memory, INTERRUPT_CONFIG_GPIO::MODE, INTERRUPT_CONFIG_GPIO::ModeVal::NEW_SAMPLE_READY);

                reg::clear(memory, FRESH_OUT_OF_RESET::FRESH_OUT_OF_RESET));
        }

        I2cDevice & i2cDevice_;
    };
}