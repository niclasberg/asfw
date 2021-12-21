#pragma once
#include "field.hpp"
#include "field_action.hpp"
#include "register_value.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, std::uint8_t shift>
        class BitIsSetAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<bool>;
            inline static constexpr bool isWrite = false;

            template<class F, class ... Results>
            decltype(auto) operator()(F && f, T value, Results... results) const
            {
                return static_cast<F&&>(f)(
                    value,
                    results..., 
                    (value & (1 << shift)) != 0);
            }
        };

        template<class _Location, class _Mask, class _RWPolicy>
        constexpr auto makeBitISSetAction()
        {
            using Value = typename _Location::Value;
            static_assert(isReadable<_RWPolicy>::value, "Field is not readable");
            static_assert(isSingleBit<_Mask>::value, "Only single bit fields can be tested");
            return BitIsSetAction<Value, _Mask::shift>{};
        }
    }

    inline constexpr struct bitIsSet_t final
    {
        template<class _Location, class _Mask, class _RWPolicy, class _ValueType>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _ValueType>) const
            -> FieldAction<_Location, decltype(detail::makeBitISSetAction<_Location, _Mask, _RWPolicy>())>
        {
            return { detail::makeBitISSetAction<_Location, _Mask, _RWPolicy>() };
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType>
            requires(!std::same_as<std::decay_t<T>, RegisterValue<_Location>>)
        decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType>) const
        {
            return static_cast<T&&>(handler)
                .readAndTransform(_Location{}, detail::makeBitISSetAction<_Location, _Mask, _RWPolicy>());
        }

        template<class Location, class Mask, class RWPolicy, class ValueType>
        INLINE bool operator()(
            RegisterValue<Location> regValue, 
            Field<Location, Mask, RWPolicy, ValueType>) const
        {
            static_assert(detail::isReadable<RWPolicy>::value, "Field is not readable");
            static_assert(detail::isSingleBit<Mask>::value, "Only single bit fields can be tested");
            return regValue.value & (1 << Mask::shift);
        }

    } bitIsSet{};
}