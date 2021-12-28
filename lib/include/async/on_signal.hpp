#pragma once
#include "scheduler.hpp"
#include "sender.hpp"
#include "receiver.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class F>
        class OnSignalSender
        {
            template<class R>
            struct OnSignalReceiver
            {
                R receiver_;
                F f_;

                template<class ... Values> 
                void setValue(Values && ... values) && 
                { 
                    async::setValue(std::move(receiver_), static_cast<Values&&>(values)...);
                }

                template<class T>
                void setSignal(T && signal)
                {
                    f_(signal);
                    async::setSignal(receiver_, static_cast<T&&>(signal));
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
                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const OnSignalReceiver<R> & self, Args &&... args) 
                    -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.receiver_, static_cast<Args&&>(args)...);
                }
            };

        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = SenderValueTypes<S, Variant, Tuple>;

            template<template<typename...> class Variant>
            using signal_types = SenderSignalTypes<S, Variant>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<S, Variant>;

            template<class S2, class F2>
            OnSignalSender(S2 && sender, F2 && f)
            : sender_(static_cast<S2&&>(sender))
            , f_(static_cast<F2&&>(f))
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, OnSignalReceiver<std::remove_cvref_t<R>>>
            {
                return async::connect(
                    std::move(sender_), 
                    OnSignalReceiver<std::remove_cvref_t<R>>{static_cast<R&&>(receiver), std::move(f_)});
            }

            template<Receiver R>
            auto connect(R && receiver) const &
                -> ConnectResultType<S, OnSignalReceiver<std::remove_cvref_t<R>>>
            {
                return async::connect(
                    sender_, 
                    OnSignalReceiver<std::remove_cvref_t<R>>{static_cast<R&&>(receiver), f_});
            }

        private:
            S sender_;
            F f_;
        };
    }

    inline constexpr struct onSignal_t
    {
        template<Sender S, class F>
        auto operator()(S && sender, F && f) const
            -> detail::OnSignalSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(sender), static_cast<F&&>(f)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<onSignal_t, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<F&&>(f));
        }
    } onSignal{};
}