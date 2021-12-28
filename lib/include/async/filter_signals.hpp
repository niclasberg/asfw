#pragma once
#include "sender.hpp"
#include "receiver.hpp"
#include <concepts>

namespace async
{
    namespace detail
    {
        template<class P, class R>
        class FilterReceiver
        {
        public:
            template<class P2, class R2>
            FilterReceiver(P2 && predicate, R2 && receiver)
            : predicate_(static_cast<P2&&>(predicate))
            , receiver_(static_cast<R2&&>(receiver))
            {

            }

            template<class T>
            void setSignal(T && signal) &
            {
                if (predicate_(signal))
                {
                    async::setSignal(receiver_, static_cast<T&&>(signal));
                }
            }

            template<class ... Ts>
            void setValue(Ts && ... values) &&
            {
                async::setValue(std::move(receiver_), static_cast<Ts&&>(values)...);
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(receiver_), static_cast<E&&>(e));
            }

            void setDone() &&
            {
                async::setDone(std::move(receiver_));
            }

        private:
            P predicate_;
            R receiver_;
        };

        template<Sender S, class P>
        class FilterSignalsSender
        {
        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = SenderValueTypes<S, Variant, Tuple>;

            template<template<typename...> class Variant>
            using signal_types = SenderSignalTypes<S, Variant>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<S, Variant>;

            template<class R, class ReceiverType = std::remove_cvref_t<R>>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, FilterReceiver<P, ReceiverType>>
            {
                return async::connect(
                    std::move(sender_),
                    FilterReceiver<P, ReceiverType>{std::move(predicate_), static_cast<R&&>(receiver)});
            }

        private:
            S sender_;
            P predicate_;
        };
    }

    inline constexpr struct filterSignals_t final
    {
        template<Sender S, class P>
        auto operator()(S && sender, P && predicate) const
            -> detail::FilterSignalsSender<std::remove_cvref_t<S>, std::remove_cvref_t<P>>
        {
            return {static_cast<S&&>(sender), static_cast<P&&>(predicate)};
        }

        template<class P>
        auto operator()(P && f) const -> BindBackResultType<filterSignals_t, P>
        {
            return bindBack(*this, static_cast<P &&>(f));
        }
    } filterSignals {};
}