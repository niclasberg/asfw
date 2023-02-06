#pragma once
#include "scheduler.hpp"
#include "sender.hpp"
#include "delegate.hpp"

namespace async
{
    namespace detail
    {
        template<Sender S, class R, TimedScheduler Sched>
        class DelayOperation
        {
            using InnerOperation = connect_result_t<S, R>;

        public:
            template<class S2, class R2>
            DelayOperation(S2 && sender, R2 && receiver, Sched & scheduler, int waitPeriodInMiliseconds)
            : scheduler_(scheduler)
            , innerOperation_(
                connect(static_cast<S2&&>(sender), static_cast<R2&&>(receiver)))
            , waitPeriodInMiliseconds_(waitPeriodInMiliseconds)
            {

            }

            void start()
            {
                scheduler_.postAfter(
                    waitPeriodInMiliseconds_, 
                    {memFn<&DelayOperation::startOperation>, *this});
            }

        private:
            int startOperation()
            {
                async::start(innerOperation_);
                return -1;
            }

            Sched & scheduler_;
            InnerOperation innerOperation_;
            int waitPeriodInMiliseconds_;
        };

        template<class S>
        class DelayFuture
        {
        public:
            using value_type = future_value_t<S>;
            using error_type = future_error_t<S>;

            template<class S2>
            DelayFuture(int waitPeriodInMiliseconds, S2 && sender)
                : waitPeriodInMiliseconds_(waitPeriodInMiliseconds)
                , sender_(static_cast<S2&&>(sender))
            {

            }

            template<class R>
            constexpr auto connect(R && receiver) &&
                -> DelayOperation<S, std::remove_cvref_t<R>, ReceiverSchedulerType<R>>
            {
                return { std::move(sender_), static_cast<R&&>(receiver), getScheduler(receiver), waitPeriodInMiliseconds_ };
            }

            template<class R>
            constexpr auto connect(R && receiver) const &
                -> DelayOperation<S, std::remove_cvref_t<R>, ReceiverSchedulerType<R>>
            {
                return { sender_, static_cast<R&&>(receiver), getScheduler(receiver), waitPeriodInMiliseconds_ };
            }

        private:
            int waitPeriodInMiliseconds_;
            S sender_;
        };
    }

    inline constexpr struct delay_t final
    {
        template<AnyFuture S>
        auto operator()(S && sender, int waitPeriod) const 
            -> detail::DelayFuture<std::remove_cvref_t<S>>
        {
            return {waitPeriod, static_cast<S&&>(sender)};
        }
    } delay{};
}