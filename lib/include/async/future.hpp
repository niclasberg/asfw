#pragma once
#include <concepts>
#include "receiver.hpp"
#include "operation.hpp"
#include "tmp/traits.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    inline constexpr struct connect_t final
    {
        template<class S, class R>
        auto operator()(S && sender, R && receiver) const 
            -> tmp::TagInvokeResultType<connect_t, S&&, R&&>
        {
            return tag_invoke(*this, static_cast<S&&>(sender), static_cast<R&&>(receiver));
        }
    } connect{};

    namespace detail
    {
        template<class S>
        concept has_future_types = requires {
            typename S::value_type;
            typename S::error_type;
        };

        template<class T, class E>
        struct ReceiverArchetype
        {
            void setValue(tmp::wrap_void_t<T> && t) &&;
            void setError(E && e) &&;
            void setDone() &&;
        };

        template<class T>
        struct ReceiverArchetype<T, void>
        {
            void setValue(tmp::wrap_void_t<T> && t) &&;
            void setDone() &&;
        };

        template<class S>
        using ReceiverArchetypeFor = detail::ReceiverArchetype<typename S::value_type, typename S::error_type>;
    }

    /**
     * A sender is a type that can be connected to 
     * a receiver to form an operation. It also 
     * provides a value_types and error_types 
     * definition, indicating the value and error
     * types, respectively that the sender may send.
     */
    template<class S>
    concept AnyFuture = 
        std::move_constructible<S> &&
        std::destructible<S> &&
        detail::has_future_types<std::remove_cvref_t<S>> &&
        requires (std::remove_cvref_t<S> && sender, detail::ReceiverArchetypeFor<S> && receiver) 
        {
            { connect(sender, receiver) } -> Operation;
        };

    template<AnyFuture S>
    using future_value_t = typename S::value_type;

    template<AnyFuture S>
    using future_error_t = typename S::error_type;

    template<class S, class T, class E>
    concept Future = 
        AnyFuture<S> &&
        std::same_as<typename S::value_type, T> &&
        (std::same_as<typename S::error_type, E> || std::same_as<typename S::error_type, void>);

    template<class F, class Receiver>
    using connect_result_t = decltype(connect(std::declval<F&&>(), std::declval<Receiver&&>()));
}