#pragma once
#include "types.hpp"
#include <tuple>
#include "reg/field_location.hpp"

/**
 * DeviceMemory: handles access to device memory for a peripheral
 * addrStart and addrEnd is the (inclusive) range
 **/
template<class T, T addrStart, T addrEnd>
struct DeviceMemory
{
    using type = volatile T;
    using ref_type = type &;
    using ptr_type = type *;

    template<T offset = 0>
    static constexpr INLINE T getAddress()
    {
        static_assert(offset < (addrEnd - addrStart + 1), "Pointer offset out of range");
        return addrStart + offset;
    }

    template<T offset>
    static INLINE void writeRegister(T value)
    {
        *std::launder(reinterpret_cast<ptr_type>(getAddress<offset>())) = value;
    }

    template<T offset>
    static INLINE T readRegister()
    {
        return *std::launder(reinterpret_cast<ptr_type>(getAddress<offset>()));
    }

    template<T offset = 0>
    static INLINE ptr_type getPtr()
    {
        return std::launder(reinterpret_cast<ptr_type>(getAddress<offset>()));
    }

    template<T offset = 0>
    static INLINE ref_type getRef()
    {
        return *getPtr<offset>();
    }
};

struct NoControl
{
    static INLINE void enable() { }
    static INLINE void disable() { }
    static INLINE void reset() { }
};

namespace detail
{
    template<class R, class ... Tail>
    auto toValueOrTuple(R r, Tail... results)
    {
        if constexpr (sizeof...(Tail) > 0)
        { 
            return std::make_tuple(r, results...);
        } 
        else
        {
            return r;
        }
    }
}

template<class Tag, class Memory, class _Control = NoControl>
class Peripheral
{
public:
    using tag = Tag;
    using type = std::remove_cv_t<typename Memory::type>;
    using ref_type = typename Memory::ref_type;

    INLINE void enable() const
    {
        _Control::enable();
    }

    INLINE void disable() const
    {
        _Control::disable();
    }

    INLINE void reset() const
    {
        _Control::reset();
    }

    template<class RegTag, type addr>
    constexpr INLINE type getAddress(reg::FieldLocation<type, RegTag, reg::FieldOffset<type, addr>>)
    {
        static_assert(std::is_same<tag, RegTag>::value, 
            "The tag of the field location did not match that of the peripheral");
        return Memory::template getAddress<addr>();
    }

    template<type addr>
    INLINE void write(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, type val) const
    {
        Memory::template writeRegister<addr>(val);
    }

    template<type addr>
    INLINE type read(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>) const
    {
        return Memory::template readRegister<addr>();
    }

    template<type addr, class F>
    INLINE void readModifyWrite(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, F && f) const
    {
        static_cast<F&&>(f)(
            [](auto val) {
                Memory::template writeRegister<addr>(val);
            },
            Memory::template readRegister<addr>());
    }

    template<type addr, class F>
    INLINE type readAndTransform(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, F && f) const
    {
        return static_cast<F&&>(f)(
            [](auto, auto... results) {
                return detail::toValueOrTuple(results...);
            },
            Memory::template readRegister<addr>());
    }

    template<type addr, class F>
    INLINE decltype(auto) readModifyWriteTransform(reg::FieldLocation<type, Tag, reg::FieldOffset<type, addr>>, F && f) const
    {
        return static_cast<F&&>(f)(
            [](auto val, auto... results) {
                Memory::template writeRegister<addr>(val);
                return detail::toValueOrTuple(results...);
            },
            Memory::template readRegister<addr>());
    }
};