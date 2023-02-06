#pragma once
#include "tmp/tag_invoke.hpp"
#include "scheduler.hpp"
#include "future.hpp"
#include "receiver.hpp"

namespace async
{
    namespace detail
    {
        template<Scheduler S, class R>
        class UseSchedulerReceiver
        {
        public:
            template<class R2>
            UseSchedulerReceiver(S & scheduler, R2 && receiver)
            : innerReceiver_(static_cast<R2&&>(receiver))
            , scheduler_(scheduler)
            {

            }

            template<class T>
            void setValue(T && value) &&
            {
                async::setValue(std::move(innerReceiver_), static_cast<T&&>(value));
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(innerReceiver_), static_cast<E&&>(e));
            }

            void setDone() &&
            {
                async::setDone(std::move(innerReceiver_));
            }

        private:
            friend S & tag_invoke(getScheduler_t, const UseSchedulerReceiver & self)
            {
                return self.scheduler_;
            }

            R innerReceiver_; 
            S & scheduler_;
        };

        template<Scheduler S, AnyFuture ParentSender>
        class UseSchedulerSender
        {
        public:
            using value_type = future_value_t<ParentSender>;
            using error_type = future_error_t<ParentSender>;

            template<class ParentSender2>
            UseSchedulerSender(S & scheduler, ParentSender2 && parentSender) 
            : parentSender_(static_cast<ParentSender2&&>(parentSender))
            , scheduler_(scheduler) 
            { 

            }

            template<class R>
            auto connect(R && receiver) &&
                -> connect_result_t<ParentSender, UseSchedulerReceiver<S, std::remove_cvref_t<R>>>
            {
                return async::connect(
                    std::move(parentSender_),
                    UseSchedulerReceiver<S, std::remove_cvref_t<R>>{scheduler_, static_cast<R&&>(receiver)});
            }
        private:
            ParentSender parentSender_;
            S & scheduler_;
        };
    }

    template<Scheduler S, class R>
    auto addSchedulerToReceiver(S & scheduler, R && receiver)
        -> detail::UseSchedulerReceiver<S, std::remove_cvref_t<R>>
    {
        return {scheduler, static_cast<R&&>(receiver)};
    }

    inline constexpr struct useScheduler_t final
    {
        template<Scheduler S, AnyFuture ParentSender>
        auto operator()(S & scheduler, ParentSender && parentSender) const
            -> detail::UseSchedulerSender<std::remove_cvref_t<S>, std::remove_cvref_t<ParentSender>>
        {
            return {scheduler, static_cast<ParentSender&&>(parentSender)};
        }
    } useScheduler{};
}