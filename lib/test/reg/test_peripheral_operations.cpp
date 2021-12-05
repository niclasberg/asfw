#include "../catch.hpp"
#include <array>
#include "reg/multi_field.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "reg/set.hpp"
#include "reg/clear.hpp"
#include "reg/toggle.hpp"
#include "reg/write.hpp"
#include "reg/apply.hpp"
#include "reg/read.hpp"

namespace
{
    struct test_tag {};
    using MockDevice = MockPeripheral<test_tag>;

    using _Location = reg::FieldLocation<std::uint32_t, test_tag, reg::FieldOffset<std::uint32_t, 0x00>>;
    constexpr auto field = reg::RWField<_Location, reg::BitMask32<0, 1>>{};
    constexpr auto field2 = reg::RWField<_Location, reg::BitMask32<1, 6>>{};
    constexpr auto multiField = reg::RWMultiField<_Location, reg::BitMask32<8, 1>, reg::RepMask<8, 1>, reg::RepLocation<3, 0x4>>{};
}

TEST_CASE( "Bits and bytes can be written", "[register]" ) {
    MockDevice device;
    resetPeripheral(device);

    SECTION("Write single bit (runtime)")
    {
        reg::write(device, field, 1);
        REQUIRE(getDeviceMemory(device, 0) == 1);       
    }

    SECTION("Write single bit (runtime, lazy)")
    {
        reg::apply(device, reg::write(field, 1));
        REQUIRE(getDeviceMemory(device, 0) == 1);       
    }

    SECTION("Write single bit (integral_constant)")
    {
        reg::write(device, field, uint32_c<1>);
        REQUIRE(getDeviceMemory(device, 0) == 1);
    }

    SECTION("Write single bit (integral_constant, lazy)")
    {
        reg::apply(device, reg::write(field, uint32_c<1>));
        REQUIRE(getDeviceMemory(device, 0) == 1);
    }

    SECTION("Write single bit (bool constant)")
    {
        reg::write(device, field, true_c);
        REQUIRE(getDeviceMemory(device, 0) == 1);
    }

    SECTION("Write multi-bit value (runtime)")
    {
        reg::write(device, field2, 0x11);
        REQUIRE(getDeviceMemory(device, 0) == 0x11 << 1);
    }

    SECTION("Write multi-bit value (runtime, lazy)")
    {
        reg::apply(device, reg::write(field2, 0x11));
        REQUIRE(getDeviceMemory(device, 0) == 0x11 << 1);
    }

    SECTION("Write multi-bit value (integral_constant)")
    {
        reg::write(device, field2, uint32_c<0x11>);
        REQUIRE(getDeviceMemory(device, 0) == 0x11 << 1);
    }

    SECTION("Write multi-bit value (integral_constant, lazy)")
    {
        reg::apply(device, reg::write(field2, uint32_c<0x11>));
        REQUIRE(getDeviceMemory(device, 0) == 0x11 << 1);
    }

    SECTION("Set bit")
    {
        reg::set(device, field);
        REQUIRE(getDeviceMemory(device, 0) == 1);
    }

    SECTION("Set bit (lazy)")
    {
        reg::apply(device, reg::set(field));
        REQUIRE(getDeviceMemory(device, 0) == 1);
    }

    SECTION("Toggle bit")
    {
        fillDeviceMemory(device, 1);
        reg::toggle(device, field);
        REQUIRE(getDeviceMemory(device, 0) == 0);
    }

    SECTION("Toggle bit (lazy)")
    {
        fillDeviceMemory(device, 1);
        reg::apply(device, reg::toggle(field));
        REQUIRE(getDeviceMemory(device, 0) == 0);
    }

    SECTION("Clear bit")
    {
        fillDeviceMemory(device, 1);
        reg::clear(device, field);
        REQUIRE(getDeviceMemory(device, 0) == 0);
    }

    SECTION("Clear bit (lazy)")
    {
        fillDeviceMemory(device, 1);
        reg::apply(device, reg::clear(field));
        REQUIRE(getDeviceMemory(device, 0) == 0);
    }

    SECTION("Write multifield")
    {
        fillDeviceMemory(device, 0);
        reg::set(device, multiField[uint8_c<0>]);
        reg::set(device, multiField[uint8_c<8>]);
        reg::set(device, multiField[uint8_c<9>]);
        reg::set(device, multiField[uint8_c<23>]);
        REQUIRE(getDeviceMemory(device, 0) == (1 << 8));
        REQUIRE(getDeviceMemory(device, 0x4) == (3 << 8));
        REQUIRE(getDeviceMemory(device, 0x8) == (1 << 15));
    }
}

TEST_CASE( "Bits and bytes can be read", "[register]" ) {
    MockDevice device;
    fillDeviceMemory(device, 0);

    SECTION("Read single bit")
    {
        setDeviceMemory(device, 0, 1);
        REQUIRE(reg::read(device, field) == 1);
    }

    SECTION("Read single bit (lazy)")
    {
        setDeviceMemory(device, 0, 1);
        REQUIRE(reg::apply(device, reg::read(field)) == 1);
    }

    SECTION("Read multi-bit value")
    {
        setDeviceMemory(device, 0, 0x11 << 1);
        REQUIRE(reg::read(device, field2) == 0x11);
    }
}