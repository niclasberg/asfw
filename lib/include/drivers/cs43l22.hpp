#pragma once
#include "cs43l22/regmap.hpp"
#include "i2c/i2c_like.hpp"
#include "i2c/i2c_memory.hpp"
#include "async/sequence.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/write.hpp"
#include "reg/apply.hpp"
#include "reg/read.hpp"
#include "reg/unchecked_write.hpp"

namespace drivers
{
    namespace cs43l22
    {
        struct DeviceIdentification
        {
            std::uint8_t deviceId;
            std::uint8_t revision;
        };
    }

    template<i2c::I2cLike I2cDevice>
    class Cs43l22
    {
        static inline constexpr std::uint8_t SLAVE_ADDRESS = 0x4A;
        template<std::uint8_t addr>
        using Loc = reg::FieldLocation<std::uint8_t, cs43l22::regmap::tag, reg::FieldOffset<std::uint8_t, addr>>;

    public:
        Cs43l22(I2cDevice & i2cDevice) : device_(i2cDevice, SLAVE_ADDRESS)
        {

        }

        async::Sender auto getDeviceIdentification()
        {
            using namespace cs43l22;
            return reg::apply(device_,
                reg::read(regmap::ID::CHIPID),
                reg::read(regmap::ID::REVID))
                | async::transform([](std::uint8_t chipId, std::uint8_t revId) {
                    return DeviceIdentification {
                        chipId, revId
                    };
                });
        }

        auto init()
        {
            using namespace cs43l22;
            using regmap::InterfaceCtl1::InterfaceFormatVal;
            using regmap::PowerCtl1::PdnVal;
            reg::RWField<Loc<0x32>, reg::BitMask8<7, 1>> mem32Bit7{};

            return async::sequence(
                // Required initialization sequence for CS43L22 (ref. DataSheet 4.9-4.11)
                reg::uncheckedWrite(device_, Loc<0>{}, 0x99U),
                reg::uncheckedWrite(device_, Loc<0x47>{}, 0x80U),
                reg::uncheckedWrite(device_, Loc<0x47>{}, 0x80U),
                reg::set(device_, mem32Bit7),
                reg::clear(device_, mem32Bit7),
                reg::uncheckedWrite(device_, Loc<0x0>{}, 0x0U),
                // Custom configuration
                reg::set(device_, regmap::ClockingCtl::AUTO),
                reg::apply(device_,
                    reg::clear(regmap::InterfaceCtl1::MASTER_SLAVE),
                    reg::clear(regmap::InterfaceCtl1::SCKL_POL),
                    reg::clear(regmap::InterfaceCtl1::DSP_MODE),
                    reg::write(regmap::InterfaceCtl1::INTERFACE_FORMAT, InterfaceFormatVal::I2S),
                    reg::write(regmap::InterfaceCtl1::WORD_LENGTH, uint8_c<3>)), // 16 bit data 
                // Power up
                reg::write(device_, regmap::PowerCtl1::PDN, PdnVal::ON));
        }

        async::Sender auto setHeadphoneVolume(std::uint8_t volume) 
        {
            // 0x00 -> 0dB 
            // 0xFF -> -0.5dB
            // 0xFE -> -1dB
            // ...
            

            using namespace cs43l22;
            return async::sequence(
                reg::write(regmap::HeadphoneAVolume::HPAVOL, volume),
                reg::write(regmap::HeadphoneBVolume::HPBVOL, volume));
        }
    private:
        i2c::I2cMemory<I2cDevice, cs43l22::regmap::tag> device_;
    };
}