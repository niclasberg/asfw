#pragma once

namespace tmp
{
    /**
     * @brief A type that can be used to trigger compilation errors by not having
     * an implementation. While this might seem like a bad idea, it's very useful
     * for debugging meta programming constructs. 
     * 
     * @example
     * Fail<std::conditional<true, bool, char>> ff;
     * 
     * will trigger a compilation error. The error diagnostics will differ from
     * compiler to compiler, but will usually include the type `T` (here bool).
     * 
     * @tparam T 
     */
    template<class ... T>
    struct Fail;

    /**
     * @brief Identity metafunction. Has a `type` alias that is always equal
     * to `T`. Useful for some meta-algorithms.
     * 
     * @tparam T 
     */
    template<class T> struct identity_ { using type = T; };

    /**
     * @brief Identity function for single parameter (and void) functions. Perfectly
     * forwards the inputs.
     * 
     */
    struct Identity
    {
        void operator()() const
        {

        }

        template<class T>
        T && operator()(T && x) const
        {
            return static_cast<T&&>(x);
        }
    };

    /**
     * @brief Represents a void value. Note: this type
     * circumvents all the awkward parts with void (e.g. not being
     * able to assign the return value from a void-returning 
     * function to a variable)
     */
    struct Void {};

    /**
     * @brief If the type parameter T is void, the returned type will
     * be Void, otherwise T
     * 
     * @tparam T 
     */
    template<class T>
    using wrap_void_t = std::conditional_t<std::is_void_v<T>, Void, T>;

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

        template<class T>
        struct CallableResult
        {
            template<class F>
            using type = decltype((std::declval<F>())(std::declval<T>()));
        };

        template<>
        struct CallableResult<void>
        {
            template<class F>
            using type = decltype((std::declval<F>())());
        };
    }

    /**
     * @brief Void or single parameter invocable objects
     * 
     * @tparam F Function object
     * @tparam T Argument type
     */
    template<class F, class T>
    concept Callable = 
        (std::is_void_v<T> && requires(F && f) { static_cast<F&&>(f)(); }) || 
        (!std::is_void_v<T> && requires(F && f, T && t) { static_cast<F&&>(f)(static_cast<T&&>(t)); });

    template<class F, class T>
    using CallableResultType = detail::CallableResult<T>::template type<F>;

    template<class F, class T, class R>
    concept CallableR = std::same_as<CallableResultType<F, T>, R>;

    template<bool condition, class T, class U>
    using ConditionalType = typename detail::ConditionalImpl<condition>::template type<T, U>;
}