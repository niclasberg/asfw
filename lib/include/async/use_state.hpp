#pragma once
#include <concepts>
#include <type_traits>
#include "future.hpp"
#include "tmp/traits.hpp"

namespace async
{
    namespace detail
    {
        template<class State, class SenderFactory, class R>
        class UseStateOperation
        {
            using InnerSender = tmp::CallableResultType<SenderFactory, State&>;
            using InnerOperation = connect_result_t<InnerSender, R>;

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
            using Self = UseStateSender<State, SenderFactory>;
            using InnerSender = tmp::CallableResultType<SenderFactory, State&>;
        public:
            using value_type = future_value_t<InnerSender>;
            using error_type = future_error_t<InnerSender>;

            template<class State2, class SenderFactory2>
            UseStateSender(State2 && state, SenderFactory2 && senderFactory)
            : state_(static_cast<State2&&>(state))
            , senderFactory_(static_cast<SenderFactory2&&>(senderFactory))
            {

            }

        private:
            template<class S, AnyReceiver R>
                requires std::same_as<std::decay_t<S>, Self>
            friend auto tag_invoke(connect_t, S && sender, R && receiver)
                -> UseStateOperation<State, SenderFactory, std::remove_cvref_t<R>>
            {
                return {
                    static_cast<S&&>(sender).state_, 
                    static_cast<S&&>(sender).senderFactory_, 
                    static_cast<R&&>(receiver)
                };
            }

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