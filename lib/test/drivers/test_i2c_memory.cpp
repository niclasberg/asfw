#include "../catch.hpp"
#include <drivers/i2c/i2c_memory.hpp>
#include "../mocks/mock_i2c.hpp"
#include "reg/write.hpp"
#include "reg/field.hpp"
#include "async/receive.hpp"

namespace 
{
    struct mock_tag{};
    template<std::uint8_t offset>
    using Location = reg::FieldLocation<std::uint8_t, mock_tag, reg::FieldOffset<std::uint8_t, offset>>;

    template<std::uint8_t offset, std::uint8_t bit, std::uint8_t size>
    constexpr auto field = reg::RWField<Location<offset>, reg::BitMask8<bit, size>>{};   
}

TEST_CASE("I2c memory")
{
    using namespace drivers;
    MockI2c mockI2c;
    const std::uint8_t SLAVE_ADDRESS = 0x4f;

    SECTION("write")
    {
        i2c::I2cMemory<MockI2c, mock_tag> memory{mockI2c, SLAVE_ADDRESS};
        auto op = 
            async::connect(
                reg::write(memory, field<0x24, 0, 8>, 0x34),
                async::receiveValue([]() { }));
        async::start(op);

        REQUIRE(mockI2c.readSlaveAddress == SLAVE_ADDRESS);
        REQUIRE(mockI2c.writeSlaveAddress == SLAVE_ADDRESS);
    }
}