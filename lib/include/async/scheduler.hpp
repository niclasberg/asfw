#pragma once
#include "delegate.hpp"
#include "tmp/tag_invoke.hpp"
#include <concepts>

namespace async
{
    inline constexpr struct schedule_t final
    {
        template<class S> 
        auto operator()(S & scheduler) const -> tmp::TagInvokeResultType<schedule_t, S>
        {
            return tag_invoke(*this, scheduler);
        }
    } schedule{};

    template<class T>
    concept Scheduler = requires(T & scheduler, Delegate<void()> delegate) {
        { scheduler.post(delegate) } -> std::same_as<bool>;
        { scheduler.postFromISR(delegate) } -> std::same_as<bool>;
        { scheduler.poll() };
    };

    template<class T>
    concept TimedScheduler = 
        Scheduler<T> &&
        requires(T & scheduler, std::uint32_t delayMs, Delegate<int()> delegate) {
            { scheduler.postAfter(delayMs, delegate) } -> std::same_as<bool>;
        };

    /**
     * Get the scheduler for a receiver
     */
    inline constexpr struct getScheduler_t final
    {
        template<class R> 
        auto operator()(const R & receiver) const 
            -> tmp::TagInvokeResultType<getScheduler_t, const R&>
        {
            return tag_invoke(*this, receiver);
        }
    } getScheduler{};

    template<class R>
    using ReceiverSchedulerType = decltype(getScheduler(std::declval<R>()));

    template<class R>
    inline constexpr bool hasScheduler = tmp::is_tag_invocable_v<getScheduler_t, std::remove_cvref_t<R>&>;

    /*namespace detail
    {
        template<class S, class R>
        class Operation
        {
        public:
            template<class S2, class R2>
            Operation(CooperativeScheduler & scheduler, S2 && sender, R2 && receiver)
            : scheduler_(scheduler)
            , operation_(connect(static_cast<S2&&>(sender), static_cast<R2&&>(receiver)))
            {

            }

            void start()
            {
                scheduler_.post([this]() {
                    operation_.start();
                });
            }

        private:
            CooperativeScheduler & scheduler_;
            ConnectResultType<S, R> operation_;
        };

        template<class S>
        struct ScheduleSender
        {
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = Variant<Tuple<>>;
            template<template<typename...> class Variant> 
            using error_types = Variant<>;

            template<VoidReceiver R>
            auto connect(R && receiver) && -> Operation<S, std::remove_cvref_t<R>>
            {
                return {std::move(sender_), static_cast<R&&>(receiver)};
            }

            S sender_;
        };
    }*/
}