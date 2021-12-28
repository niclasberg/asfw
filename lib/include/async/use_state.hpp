#pragma once
#include <concepts>
#include <type_traits>
#include "sender.hpp"
#include "tmp/traits.hpp"

namespace async
{
    namespace detail
    {
        template<class State, class SenderFactory, class R>
        class UseStateOperation
        {
            using InnerSender = tmp::CallableResultType<SenderFactory, State&>;
            using InnerOperation = ConnectResultType<InnerSender, R>;

        public:
            template<class State2, class SenderFactory2, class R2>
            UseStateOperation(State2 && state, SenderFactory2 && senderFactory, R2 && receiver)
            : state_(static_cast<State2&&>(state))
            , senderFactory_(static_cast<SenderFactory2&&>(senderFactory))
            , innerOperation_(
                connect(senderFactory_(state_), static_cast<R2&&>(receiver)))
            {

            }

            UseStateOperation(const UseStateOperation&) = delete;
            UseStateOperation & operator=(const UseStateOperation&) = delete;

            void start()
            {
                async::start(innerOperation_);
            }

        private:
            State state_;
            SenderFactory senderFactory_;
            InnerOperation innerOperation_;
        };

        template<class State, std::invocable<State&> SenderFactory>
        class UseStateSender
        {
            using InnerSender = tmp::CallableResultType<SenderFactory, State&>;
        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = SenderValueTypes<InnerSender, Variant, Tuple>;

            template<template<typename...> class Variant>
            using signal_types = SenderSignalTypes<InnerSender, Variant>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<InnerSender, Variant>;

            template<class State2, class SenderFactory2>
            UseStateSender(State2 && state, SenderFactory2 && senderFactory)
            : state_(static_cast<State2&&>(state))
            , senderFactory_(static_cast<SenderFactory2&&>(senderFactory))
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> UseStateOperation<State, SenderFactory, std::remove_cvref_t<R>>
            {
                return {std::move(state_), std::move(senderFactory_), static_cast<R&&>(receiver)};
            }

            template<class R>
            auto connect(R && receiver) const &
                -> UseStateOperation<State, SenderFactory, std::remove_cvref_t<R>>
            {
                return {state_, senderFactory_, static_cast<R&&>(receiver)};
            }

        private:
            State state_;
            SenderFactory senderFactory_;
        };
    }

    constexpr inline struct useState_t
    {
        template<class T, std::invocable<T&> SF>
        auto operator()(T && state, SF && senderFactory) const
            -> detail::UseStateSender<std::remove_cvref_t<T>, std::remove_cvref_t<SF>>
        {
            return {static_cast<T&&>(state), static_cast<SF&&>(senderFactory)};
        }
    } useState{};
}