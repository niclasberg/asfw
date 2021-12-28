#pragma once
#include "sender.hpp"
#include "receiver.hpp"
#include "bind_back.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class F>
        class SignalOnValueSender
        {
            template<class R>
            struct SignalOnValueReceiver
            {
                template<class ... Values>
                void setValue(Values && ... values) &&
                {
                    async::setSignal(receiver_, std::move(transform_)(static_cast<Values&&>(values)...));
                    async::setValue(std::move(receiver_));
                }

                template<class T>
                void setSignal(T && signal)
                {
                    async::setSignal(receiver_, static_cast<T&&>(signal));
                }

                template<class E>
                void setError(E && error) &&
                {
                    async::setError(std::move(receiver_), static_cast<E&&>(error));
                }

                void setDone() &&
                {
                    async::setDone(std::move(receiver_));
                }

                R receiver_;
                F transform_;
            private:
                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const SignalOnValueReceiver<R> & self, Args &&... args) 
                    -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.receiver_, static_cast<Args&&>(args)...);
                }
            };

            template<class ... Values>
            using SignalType = tmp::CallableResultType<F, Values...>;

        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = Variant<Tuple<>>;

            template<template<typename...> class Variant>
            using signal_types = tmp::apply_<
                tmp::unique_<
                    tmp::concat_<
                        SenderValueTypes<S, tmp::TypeList, SignalType>,
                        SenderSignalTypes<S, tmp::TypeList>>>,
                Variant>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<S, Variant>;

            template<class S2, class F2>
            SignalOnValueSender(S2 && sender, F2 && transform)
            : sender_(static_cast<S2&&>(sender))
            , transform_(static_cast<F2&&>(transform))
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, SignalOnValueReceiver<std::remove_cvref_t<R>>>
            {
                return async::connect(
                    std::move(sender_), 
                    SignalOnValueReceiver<std::remove_cvref_t<R>>{static_cast<R&&>(receiver), std::move(transform_)});
            }

            template<class R>
            auto connect(R && receiver) const &
                -> ConnectResultType<S, SignalOnValueReceiver<std::remove_cvref_t<R>>>
            {
                return async::connect(
                    sender_, 
                    SignalOnValueReceiver<std::remove_cvref_t<R>>{static_cast<R&&>(receiver), transform_});
            }

        private:
            S sender_;
            F transform_;
        };
    }

    inline constexpr struct signalOnValue_t final
    {
        template<Sender S, class F>
        auto operator()(S && sender, F && transform) const
            -> detail::SignalOnValueSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(sender), static_cast<F&&>(transform)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<signalOnValue_t, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } signalOnValue {};
}