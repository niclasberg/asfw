#pragma once
#include <boost/hana/type.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/minus.hpp>
#include <boost/hana/mult.hpp>
#include <boost/hana/div.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/less.hpp>
#include <boost/hana/less_equal.hpp>
#include <boost/hana/greater.hpp>
#include <boost/hana/greater_equal.hpp>
#include <cstdint>

#ifdef __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif

namespace hana = boost::hana;

using hana::integral_constant;
using hana::integral_c;

template<int value> using int_ = integral_constant<int, value>;
template<std::uint32_t value> using uint32_ = integral_constant<std::uint32_t, value>;
template<std::uint16_t value> using uint16_ = integral_constant<std::uint16_t, value>;
template<std::uint8_t value> using uint8_ = integral_constant<std::uint8_t, value>;

template<int value> constexpr int_<value> int_c{};
template<std::uint32_t value> constexpr uint32_<value> uint32_c{};
template<std::uint16_t value> constexpr uint16_<value> uint16_c{};
template<std::uint8_t value> constexpr uint8_<value> uint8_c{};

using hana::false_;
using hana::true_;
using hana::false_c;
using hana::true_c;
using hana::bool_c;
using hana::bool_;
using hana::type_c;

// Integral constant from enum
#define ENUM_C(E) integral_c<decltype(E), E>

template<auto value>
using constant_ = integral_constant<decltype(value), value>;

template<auto value>
constexpr constant_<value> constant_c{};

// Gpio pin identifier
template<std::uint8_t portNo, std::uint8_t pinNo>
struct Pin
{

};

template<std::uint8_t portNo, std::uint8_t pinNo>
constexpr Pin<portNo, pinNo> PIN {};

// General device id
template<std::uint8_t> struct DeviceId {};

template<std::uint8_t id> 
constexpr DeviceId<id> deviceId{};


// Interrupt identifier
template<int number> struct Interrupt : int_<number> { };