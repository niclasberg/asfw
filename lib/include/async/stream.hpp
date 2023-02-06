#pragma once
#include <type_traits>
#include "receiver.hpp"
#include "operation.hpp"

namespace async
{
    inline constexpr struct subscribe_t final
    {
        template<class S, class R>
        auto operator()(S && stream, R && receiver) const 
            -> tmp::TagInvokeResultType<subscribe_t, S&&, R&&>
        {
            return tag_invoke(*this, static_cast<S&&>(stream), static_cast<R&&>(receiver));
        }
    } subscribe{};

    namespace detail
    {
        template<class S>
        concept HasStreamTypes = requires {
            typename S::value_type;
            typename S::error_type;
        };

        template<class T, class E>
        struct StreamReceiverArchetype
        {
            void setNext(tmp::wrap_void_t<T> && t) &;
            void setError(E && e) &&;
            void setDone() &&;
        };

        template<class T>
        struct StreamReceiverArchetype<T, void>
        {
            void setNext(tmp::wrap_void_t<T> && t) &;
            void setDone() &&;
        };

        template<class S>
        using StreamReceiverArchetypeFor = detail::StreamReceiverArchetype<typename S::value_type, typename S::error_type>;
    }

    template<class S>
    concept AnyStream = 
        std::move_constructible<S> &&
        std::destructible<S> &&
        detail::HasStreamTypes<std::remove_cvref_t<S>> &&
        requires(std::remove_cvref_t<S> && stream, detail::StreamReceiverArchetypeFor<S> && receiver) {
            { subscribe(stream, receiver) } -> StreamOperation;
        };

    template<AnyStream S>
    using stream_value_t = typename S::value_type;

    template<AnyStream S>
    using stream_error_t = typename S::error_type;

    template<class S, class T, class E>
    concept Stream = 
        AnyStream<S> &&
        std::same_as<typename S::value_type, T> &&
        std::same_as<typename S::error_type, E>;

    template<class S, class R>
    using subscribe_result_t = decltype(subscribe(std::declval<S&&>(), std::declval<R&&>()));
}