#pragma once
#include "field_action.hpp"
#include <type_traits>
#include "tmp/type_list.hpp"

namespace reg
{
    namespace detail
    {
        template<class ... Args> struct pack_t{};

        template<class ... Actions>
        class CombinedAction : Actions...
        {
            template<class F, class Head, class... Tail, class ... Results>
            static constexpr decltype(auto) callRecursive(const CombinedAction * self, F && f, pack_t<Head, Tail...>, Results... args)
            {
                if constexpr (sizeof...(Tail) > 0)
                {
                    return self->Head::operator()(
                        [self, f = static_cast<F&&>(f)](auto ... args2) 
                        {
                            return callRecursive(self, f, pack_t<Tail...>{}, args2...);
                        }, 
                        args...);
                }
                else
                {
                    return self->Head::operator()(
                        [f = static_cast<F&&>(f)](auto ... args2) 
                        {
                            return f(args2...);
                        },
                        args...);
                }
            }

        public:
            template<template<typename...> class Tuple>
            using ValueTypes = tmp::apply_<
                tmp::concat_<typename Actions::template ValueTypes<tmp::TypeList>...>, 
                Tuple>;
            inline static constexpr bool isWrite = (Actions::isWrite || ... );

            CombinedAction(const Actions & ... actions) : Actions(actions)... {}

            template<class F, class ... Results>
            constexpr decltype(auto) operator()(F && f, Results... results) const
            {
                return callRecursive(this, static_cast<F&&>(f), pack_t<Actions...>{}, results...);
            }
        };

        template<class ... Actions>
        constexpr auto makeCombinedAction(Actions && ... actions) -> CombinedAction<std::remove_cvref_t<Actions>...>
        {
            return {static_cast<Actions&&>(actions)...};
        }
    }

    inline constexpr struct combine_t
    {
        template<class _Location, class ... Actions>
        constexpr auto operator()(FieldAction<_Location, Actions> && ... fieldActions) const
            -> FieldAction<_Location, detail::CombinedAction<std::remove_cvref_t<Actions>...>>
        {
            return { detail::makeCombinedAction(static_cast<Actions&&>(fieldActions)...) };
        }
    } combine{};
}