#pragma once
#include <type_traits>
#include "tmp/tag_invoke.hpp"

namespace async
{
    template<class S>
    concept StopToken = requires(S && s) {
        { s.stopPossible() } -> std::convertible_to<bool>;
        { s.stopRequested() } -> std::convertible_to<bool>;
    };

    struct UnstoppableToken
    {
        template<class F>
        struct CallbackType
        {
            explicit CallbackType(UnstoppableToken, F &&) { }
        };

        constexpr bool stopPossible() const { return false; }
        constexpr bool stopRequested() const { return false; }
    };

    /**
     * Get the stop token for a receiver
     */
    inline constexpr struct getStopToken_t 
    {
        template<class R> 
            requires tmp::TagInvocable<getStopToken_t, R>
        auto operator()(R & receiver) const -> tmp::TagInvokeResultType<getStopToken_t, R>
        {
            return tag_invoke(*this, receiver);
        }

        template<class R> 
            requires (!tmp::TagInvocable<getStopToken_t, R>)
        auto operator()(R &) const -> UnstoppableToken
        {
            return {};
        }
    } getStopToken {};

    template<class Receiver>
    struct stop_token_type_for
    {
        using type = decltype(getStopToken(std::declval<Receiver &>()));
    };

    template<class Receiver>
    using stop_token_type_for_t = typename stop_token_type_for<Receiver>::type;
}