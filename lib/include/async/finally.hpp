#pragma once
#include <type_traits>
#include <concepts>
#include "sender.hpp"
#include "bind_back.hpp"
#include "connect.hpp"

namespace async
{
    namespace detail
    {
        template<class R, class F>
        class FinallyReceiver
        {
        public:
            template<class ... Values>
                requires ReceiverOf<R, Values...>
            void setValue(Values && ... values) &&
            {
                f_();
                async::setValue(std::move(receiver));
            }

            template<class ... Values>
                requires ManyReceiverOf<R, Values...>
            void setNext(Values && ... values)
            {
                async::setNext(receiver, static_cast<Values &&>(values)...);
            }

            template<class E>
            void setError(E && e) &&
            {
                f_();
                async::setError(std::move(receiver), static_cast<E &&>(e));
            }

            void setDone() &&
            {
                f_();
                async::setDone(std::move(receiver));
            }

        private:
            R receiver_;
            F f_;
        };

        template<class S, class F>
        class FinallySender
        {
            
            template<template<typename...> typename Tuple>
            using error_types = typename S::template error_types<Tuple>;

            template<class R>
            connect_result_t<S, FinallyReceiver<R, F>> operator()(R && receiver)
            {
                return connect(static_cast<R&&>(receiver))
            }
        };
    }

    /**
     * Attaches a callback to a Sender or a ManySender that is invoked
     * right before the sender completes.
     * 
     * Note: The callback is invoked regardless of termination signal (
     * value, error or done).
     */
    inline constexpr struct finally_t
    {
        template<class S, class F>
            requires (Sender<S> || ManySender<S>) && std::invocable<F>
        auto operator()(S && s, F && f) const
            -> detail::FinallySender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(s), static_cast<F&&>(f)};
        }

        template<std::invocable F>
        auto operator()(F && f) const
            -> bindBack_result_t<finally_t, F>
        {
            return bindBack(*this, static_cast<F&&>(f));
        }
    } finally{};
}