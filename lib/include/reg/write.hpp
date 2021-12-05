#pragma once
#include "field_action.hpp"
#include "field.hpp"
#include "set.hpp"
#include "clear.hpp"
#include "tmp/traits.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, T clearMask, T setMask>
        class ConstClearAndWriteAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
            static inline constexpr bool isWrite = true;

            template<class F, class ... Args>
            decltype(auto) operator()(F && f, T value, Args... args) const
            {
                value = (value & ~clearMask) | setMask;
                return static_cast<F&&>(f)(value, args...);
            }
        };

        template<class T, T clearMask>
        class ClearAndWriteAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
            static inline constexpr bool isWrite = true;

            template<class F, class ... Args>
            decltype(auto) operator()(F && f, T value, Args... args) const
            {
                value = (value & ~clearMask) | setMask;
                return static_cast<F&&>(f)(value, args...);
            }

            T setMask;
        };

        template<class T>
        constexpr std::underlying_type_t<T> getValue(T value, std::true_type)
        {
            return static_cast<std::underlying_type_t<T>>(value);
        }

        template<class T>
        constexpr T getValue(T value, std::false_type)
        {
            return value;
        }

        template<class _Location, class _Mask, class _RWPolicy, class _Value>
        constexpr auto makeWriteAction(
            Field<_Location, _Mask, _RWPolicy, _Value>, 
            typename Field<_Location, _Mask, _RWPolicy, _Value>::Value value)
        {
            using UnderlyingType = typename _Location::Value;
            static_assert(detail::isWriteable<_RWPolicy>::value, "Field is not writeable");
            UnderlyingType mask = getValue(value, std::is_enum<_Value>{}) << _Mask::shift;
            
            return ClearAndWriteAction<UnderlyingType, _Mask::value>{mask};
        }

        template<class _Location, class _Mask, class _RWPolicy, class _Value, _Value value>
        constexpr auto makeWriteAction(
            Field<_Location, _Mask, _RWPolicy, _Value>, 
            integral_constant<_Value, value>)
        {
            static_assert(detail::isWriteable<_RWPolicy>::value, "Field is not writeable");
            
            using UnderlyingType = typename _Location::Value;
            constexpr UnderlyingType val = detail::getValue(value, std::is_enum<_Value>{});
            static_assert(val <= _Mask::value, "Field value out of range");

            return ConstClearAndWriteAction<UnderlyingType, _Mask::value, val << _Mask::shift>{};
        }

        template<class _Location, class _Mask, class _RWPolicy, class _Value>
        constexpr auto makeWriteAction(Field<_Location, _Mask, _RWPolicy, _Value>, true_)
        {
            return makeSetAction<_Location, _Mask, _RWPolicy>();
        }

        template<class _Location, class _Mask, class _RWPolicy, class _Value>
        constexpr auto makeWriteAction(Field<_Location, _Mask, _RWPolicy, _Value>, false_)
        {
            return makeClearAction<_Location, _Mask, _RWPolicy>();
        }
    }

    inline constexpr struct write_t
    {
        template<class _Location, class _Mask, class _RWPolicy, class _Value, class Value>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _Value> field, Value value) const
            -> FieldAction<_Location, decltype(detail::makeWriteAction(field, value))>
        {
            return {detail::makeWriteAction(field, value)};
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType, class Value>
        INLINE decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType> field, Value value) const
        {
            return static_cast<T&&>(handler)
                .readModifyWrite(_Location{}, detail::makeWriteAction(field, value));
        }
    } write{};
}