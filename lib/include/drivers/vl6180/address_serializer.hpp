#pragma once

namespace drivers::vl6180
{
    struct AddressSerializer
    {
        static void serializeAddress(std::uint16_t t, std::uint8_t * buf)
        {
            buf[0] = (t >> 8) & 0xff;
            buf[1] = t & 0xff;
        }
    };
}