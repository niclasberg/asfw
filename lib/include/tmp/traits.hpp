#pragma once

namespace tmp
{
    template<class ... T>
    struct Fail;

    template<class T>
    struct Identity
    {
        using type = T;
    };

    namespace detail
    {
        template<bool condition>
        struct ConditionalImpl
        {
            template<class T, class U>
            using type = T;
        };

        template<>
        struct ConditionalImpl<false>
        {
            template<class T, class U>
            using type = U;
        };
    }

    template<bool condition, class T, class U>
    using ConditionalType = typename detail::ConditionalImpl<condition>::template type<T, U>;

    template<class F, class ... Args>
    using CallableResultType = decltype(std::declval<F>()(std::declval<Args>()...));
}