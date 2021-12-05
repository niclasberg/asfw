#pragma once
#include "field.hpp"
#include "field_action.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, T _xorMask>
        struct ToggleAction
        {
            template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
            static inline constexpr bool isWrite = true;

            template<class F, class ... Args>
            decltype(auto) operator()(F && f, T value, Args... args) const
            {
                return static_cast<F&&>(f)(value ^ _xorMask, args...);
            }        
        };

        template<class _Location, class _Mask, class _RWPolicy>
        constexpr auto makeToggleAction()
        {
            using Value = typename _Location::Value;
            static_assert(isWriteable<_RWPolicy>::value, "Field is not writeable");
            static_assert(isSingleBit<_Mask>::value, "Only single bit fields can be toggled");
            return ToggleAction<Value, 1U << _Mask::shift>{};
        }
    }

    inline constexpr struct toggle_t
    {
        template<class _Location, class _Mask, class _RWPolicy, class _ValueType>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _ValueType>) const
            -> FieldAction<_Location, decltype(detail::makeToggleAction<_Location, _Mask, _RWPolicy>())>
        {
            return {detail::makeToggleAction<_Location, _Mask, _RWPolicy>()};
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType>
        INLINE decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType>) const
        {
            return static_cast<T&&>(handler)
                .readModifyWrite(_Location{}, detail::makeToggleAction<_Location, _Mask, _RWPolicy>());
        }
    } toggle{};
}