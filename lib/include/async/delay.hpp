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
            using InnerOperation = ConnectResultType<S, R>;

        public:
            template<class S2, class R2>
            DelayOperation(S2 && sender, R2 && receiver, Sched & scheduler, int waitPeriodInMiliseconds)
            : scheduler_(scheduler)
            , innerOperation_(
                connect(static_cast<S2&&>(sender), static_Cast<R2&&>(receiver)))
            , waitPeriodInMiliseconds_(waitPeriodInMiliseconds)
            {

            }

            void start()
            {
                scheduler_.postAfter(
                    waitPeriodInMiliseconds_, 
                    {memFn<&DelayOperation::startOperation>, this});
            }

        private:
            int startOperation()
            {
                async::start(innerOperation_);
                return -1;
            }

            Sched & scheduler_;
            InnerOperation innerOperation_;
            int waitPeriodInMiliseconds_ = 1;
        };

        template<Sender S>
        class DelaySender
        {
        public:
            template<class S2>
            DelaySender(int waitPeriodInMiliseconds, S2 && sender)
            : waitPeriodInMiliseconds_(waitPeriodInMiliseconds)
            , sender_(static_cast<S2&&>(sender))
            {

            }

            template<class R>
            constexpr auto connect(R && receiver) &&
                -> DelayOperation<S, std::remove_cvref_t<R>, ReceiverSchedulerType<R>>
            {
                return { 
                    std::move(sender), 
                    static_cast<R&&>(receiver), 
                    getScheduler(receiver), 
                    waitPeriodInMiliseconds_ };
            }

        private:
            int waitPeriodInMiliseconds_ = 1;
            S sender_;
        };
    }

    inline constexpr struct delay_t final
    {
        template<VoidSender S>
        auto operator()(S && sender) const -> detail::DelaySender<std::remove_cvref_t<S>>
        {
            return 
        }
    } delay{};
}