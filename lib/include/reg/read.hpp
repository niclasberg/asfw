#pragma once
#include "field.hpp"
#include "field_action.hpp"

namespace reg
{
    namespace detail
    {
        template<class T, T mask, std::uint8_t shift>
        class ReadAction
        {
        public:
            template<template<typename...> class Tuple> using ValueTypes = Tuple<T>;
            inline static constexpr bool isWrite = false;

            template<class F, class ... Results>
            decltype(auto) operator()(F && f, T value, Results... results) const
            {
                return static_cast<F&&>(f)(
                    value,
                    results..., 
                    (value & mask) >> shift);
            }
        };

        template<class _Location, class _Mask, class _RWPolicy>
        constexpr auto makeReadAction()
        {
            using Value = typename _Location::Value;
            static_assert(detail::isReadable<_RWPolicy>::value, "Field is not readable");
            return ReadAction<Value, _Mask::type::value, _Mask::shift>{};
        }
    }

    inline constexpr struct read_t
    {
        template<class _Location, class _Mask, class _RWPolicy, class _ValueType>
        constexpr auto operator()(Field<_Location, _Mask, _RWPolicy, _ValueType>) const
            -> FieldAction<_Location, decltype(detail::makeReadAction<_Location, _Mask, _RWPolicy>())>
        {
            return { detail::makeReadAction<_Location, _Mask, _RWPolicy>() };
        }

        template<class T, class _Location, class _Mask, class _RWPolicy, class _ValueType>
        decltype(auto) operator()(T && handler, Field<_Location, _Mask, _RWPolicy, _ValueType>) const
        {
            return static_cast<T&&>(handler)
                .readAndTransform(_Location{}, detail::makeReadAction<_Location, _Mask, _RWPolicy>());
        }

    } read{};
}