#pragma once
#include "field.hpp"
#include "field_action.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, T clearMask>
        class ClearAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<>;
            inline static constexpr bool isWrite = true;

            template<class F, class ... Args>
            decltype(auto) operator()(F && f, T value, Args... args) const
            {
                value &= ~clearMask;
                return static_cast<F&&>(f)(value, args...);
            }
        };

        template<class _Location, class _Mask, class _RWPolicy>
        constexpr auto makeClearAction()
        {
            using Value = typename _Location::Value;
            static_assert(detail::isWriteable<_RWPolicy>::value, "Field is not writeable");
            static_assert(detail::isSingleBit<_Mask>::value, "Only single bit fields can be cleared");
            return ClearAction<Value, (1U << _Mask::shift)>{};
        }
    }

    inline constexpr struct clear_t
    {
        template<class _Location, class _Mask, class _RWPolicy, class _ValueType>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _ValueType>) const
            -> FieldAction<_Location, decltype(detail::makeClearAction<_Location, _Mask, _RWPolicy>())>
        {
            return {detail::makeClearAction<_Location, _Mask, _RWPolicy>()};
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType>
        decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType>) const
        {
            return static_cast<T&&>(handler)
                .readModifyWrite(_Location{}, detail::makeClearAction<_Location, _Mask, _RWPolicy>());
        }
    } clear{};
}