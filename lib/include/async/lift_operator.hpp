#pragma once
#include "sender.hpp"
#include "receiver.hpp"
#include "bind_back.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class R, class F>
        class LiftOperation
        {
            struct LiftReceiver
            {
            public:
                LiftReceiver(LiftOperation & parent)
                : parent_(parent)
                {

                }

                template<class ... Values>
                void setValue(Values && ... values) &&
                {
                    liftedOperation_(setValue, static_cast<Values&&>(values)...);
                }

                template<class E>
                void setError(E && e) &&
                {
                    liftedOperation_(setError, static_cast<E&&>(e));
                }

                void setDone() &&
                {
                    liftedOperation_(start, std::move(innerReceiver_));
                }

            private:
                LiftOperation & parent_;
            };

        public:
            template<class S2, class R2, class F2>
            LiftOperation(S2 && sender, R2 && receiver, F2 && liftedOperation)
            : sender_(static_cast<S2&&>(sender))
            , receiver_(static_cast<R2&&>(receiver))
            , liftedOperation_(static_cast<F2&&>(liftedOperation))
            {

            }

        private:
            S sender_;
            R receiver_;
            F liftedOperation_;
        };

        template<class S, class F>
        class LiftSender
        {
        public:
            template<template<typename...> class Variant, template<template...> class Tuple>
            using value_types = typename F::template value_types<S, Variant, Tuple>;

            template<template<typename...> class Variant>
            using error_types = typename F::template error_types<S, Variant>;

            template<class R>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, LiftReceiver<std::remove_cvref_t<R>, F>>
            {
                return async::connect(
                    std::move(sender_), 
                    LiftReceiver<std::remove_cvref_t<R>, F>{static_cast<R&&>(receiver), std::move(liftedOperation_)});
            }

        private:
            S sender_;
            F liftedOperation_;
        };
    }

    inline constexpr struct liftOperator_t final
    {
        template<Sender S, class F>
        auto operator()(S && sender, F && operationFactory) const
            -> detail::LiftSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(sender), static_cast<F&&>(operationFactory)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<liftOperator_t, F>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } liftOperator {};
}