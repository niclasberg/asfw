#pragma once
#include "traits.hpp"

namespace tmp
{
    /**
     * Extract the last type in a parameter pack
     */
    template<typename... Ts>
    using LastType = typename decltype((Identity<Ts>{}, ...))::type;

    /**
     * Extract the first type in a parameter pack
     */
    template<typename T, typename... Ts>
    using FirstType = T;

    template<class T, class ... Ts>
    constexpr bool containsType = (std::is_same_v<T, Ts> || ...);
}