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

    template<class T>
    inline constexpr bool is_type_list_v = false;

    template<class ... Ts>
    inline constexpr bool is_type_list_v<TypeList<Ts...>> = true;

    /**
     * @brief Concatanate two type lists
     * 
     * @tparam Ls 
     * @tparam Rs 
     * @return TypeList<Ls..., Rs...> 
     */
    template<class ... Ls, class ... Rs>
    constexpr TypeList<Ls..., Rs...> operator+(TypeList<Ls...>, TypeList<Rs...>)
    {
        return {};
    }

    template<class ... Ts>
        requires (is_type_list_v<Ts> && ...)
    constexpr auto concat(Ts ... lists)
    {
        return (lists + ...);
    }

    template<class T, template<typename...> class F>
    using apply_ = typename T::template apply<F>;

    template<class ... Ts>
    using concat_ = decltype(concat(std::declval<Ts>()...));

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