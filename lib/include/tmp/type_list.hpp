#pragma once
#include "traits.hpp"

namespace tmp
{
    template<class... Ts>
    struct TypeList
    {
        template<template<typename...> class F>
        using apply = F<Ts...>;
    };

    template<class T, template<typename...> class F>
    using apply_ = typename T::template apply<F>;

    namespace detail
    {
        template<class ... Ts> struct ConcatImpl;

        template<class ... Ts>
        struct ConcatImpl<TypeList<Ts...>>
        {
            using type = TypeList<Ts...>;
        };

        template<class ... T1, class ... T2, class ... Tail>
        struct ConcatImpl<TypeList<T1...>, TypeList<T2...>, Tail...> : ConcatImpl<TypeList<T1..., T2...>, Tail...> { };

        /*template<class ... T1, class ... T2, class ... T3, class ... T4, class ... Tail>
        struct ConcatImpl<
            TypeList<T1...>, TypeList<T2...>, TypeList<T3...>, TypeList<T4...>, Tail...>
        : ConcatImpl<TypeList<T1..., T2..., T3..., T4...>, Tail...> { };

        template<class ... T1, class ... T2, class ... T3, class ... T4, class ... T5, class ... T6, class ... T7, class ... T8, class ... Tail>
        struct ConcatImpl<
            TypeList<T1...>, TypeList<T2...>, TypeList<T3...>, TypeList<T4...>, 
            TypeList<T5...>, TypeList<T6...>, TypeList<T7...>, TypeList<T8...>, Tail...>
        : ConcatImpl<TypeList<T1..., T2..., T3..., T4..., T5..., T6..., T7..., T8...>, Tail...> { };*/
    }

    template<class ... Ts>
    using concat_ = typename detail::ConcatImpl<Ts...>::type;

    namespace detail
    {
        template<class... Ts> struct UniqueImpl;
        
        template<class... Ts>
        struct UniqueImpl<TypeList<>, Ts...> 
        {
            using type = TypeList<Ts...>;
        };;

        template<class U, class... Us, class... Ts>
        struct UniqueImpl<TypeList<U, Us...>, Ts...> 
            : ConditionalType<
                (std::is_same_v<U, Ts> || ...), 
                UniqueImpl<TypeList<Us...>, Ts...   >,
                UniqueImpl<TypeList<Us...>, Ts..., U>> {};
    }

    template<class... Ts>
    using unique_ = typename detail::UniqueImpl<Ts...>::type;

    namespace detail
    {
        template<class T, template<typename> class F> struct TransformImpl;

        template<template<typename> class F, class ... Ts> 
        struct TransformImpl<TypeList<Ts...>, F>
        {
            using type = TypeList<F<Ts>...>;
        };
    }

    template<class T, template<typename> class F>
    using transform = typename detail::TransformImpl<T, F>::type;

    namespace detail
    {
        template<template<typename...> class F, template<typename...> class G>
        struct NestedApplyImpl
        {
            template<typename... Lists>
            using apply = F<typename Lists::template apply<G>...>;
        };
    }
    
    template<class ListLists, template<typename...> class F, template<typename...> class G>
    using nestedApply_ = typename ListLists::template apply<detail::NestedApplyImpl<F, G>::template apply>;
}