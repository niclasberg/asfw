#pragma once
#include "field.hpp"
#include "field_action.hpp"
#include "types.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, T setMask>
        class SetAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
            inline static constexpr bool isWrite = true;

            template<class F, class ... Results>
            constexpr decltype(auto) operator()(F && f, T value, Results... results) const
            {
                value |= setMask;
                return static_cast<F&&>(f)(value, results...);
            }
        };

        template<class _Location, class _Mask, class _RWPolicy>
        constexpr auto makeSetAction()
        {
            using Value = typename _Location::Value;
            static_assert(isWriteable<_RWPolicy>::value, "Field is not writeable");
            static_assert(isSingleBit<_Mask>::value, "Only single bit fields can be set");
            return SetAction<Value, 1U << _Mask::shift>{};
        }
    }

    inline constexpr struct set_t
    {
        template<class _Location, class _Mask, class _RWPolicy, class _ValueType>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _ValueType>) const
            -> FieldAction<_Location, decltype(detail::makeSetAction<_Location, _Mask, _RWPolicy>())>
        {
            return {detail::makeSetAction<_Location, _Mask, _RWPolicy>()};
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType>
        decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType>) const
        {
            return static_cast<T&&>(handler)
                .readModifyWrite(_Location{}, detail::makeSetAction<_Location, _Mask, _RWPolicy>());
        }
    } set{};
}

