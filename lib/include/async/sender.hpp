#pragma once
#include <concepts>
#include "tmp/traits.hpp"

namespace async
{
    namespace detail
    {
        template <template <template <typename...> class, template <typename...> class> class>
        struct _has_value_types;

        template <template <template <typename...> class> class>
        struct _has_error_types;

        template <template <template <typename...> class> class>
        struct _has_signal_types;

        template<class S>
        concept has_sender_types = requires {
            typename detail::_has_value_types<S::template value_types>;
            typename detail::_has_error_types<S::template error_types>;
            typename detail::_has_signal_types<S::template signal_types>;
        };
    }

    inline constexpr struct start_t final
    {
        template<class Op>
        void operator()(Op & operation) const
        {
            operation.start();
        }
    } start{};

    template<class T>
    concept Operation = requires(T & operation) {
        {start(operation)};
    };

    /**
     * A sender is a type that can be connected to 
     * a receiver to form an operation. It also 
     * provides a value_types and error_types 
     * definition, indicating the value and error
     * types, respectively that the sender may send.
     */
    template<class S>
    concept Sender = 
        std::move_constructible<S> &&
        detail::has_sender_types<std::remove_cvref_t<S>>;
    
    template<Sender S, template <typename...> class Variant>
    using SenderErrorTypes = typename S::template error_types<Variant>;

    template<Sender S, 
        template <typename...> class Variant,
        template <typename...> class Tuple>
    using SenderValueTypes = typename S::template value_types<Variant, Tuple>;

    template<Sender S, template <typename...> class Variant>
    using SenderSignalTypes = typename S::template signal_types<Variant>;

    inline constexpr struct connect_t final
    {
        template<Sender S, class R>
        constexpr decltype(auto) operator()(S && sender, R && receiver) const
        {
            return static_cast<S&&>(sender).connect(static_cast<R&&>(receiver));
        }

    } connect{};

    template<class Sender, class Receiver>
    using ConnectResultType = decltype(connect(std::declval<Sender>(), std::declval<Receiver>()));
}