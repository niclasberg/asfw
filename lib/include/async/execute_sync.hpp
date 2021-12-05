#pragma once
#include "sender.hpp"
#include "scheduler.hpp"

namespace async
{
    enum class ExecutionStatus
    {
        RUNNING,
        COMPLETED_WITH_VALUE,
        COMPLETED_WITH_ERROR,
        COMPLETED_WITH_DONE
    };

    namespace detail
    {
        template<Scheduler S>
        class ExecuteSyncReceiver
        {
        public:
            ExecuteSyncReceiver(S & scheduler, ExecutionStatus & status)
            : scheduler_(scheduler)
            , status_(status)
            {

            }

            void setValue() &&
            {
                status_ = ExecutionStatus::COMPLETED_WITH_VALUE;
            }

            void setNext()
            {
                // Do nothing
            }

            template<class E>
            void setError(E && e) &&
            {
                status_ = ExecutionStatus::COMPLETED_WITH_ERROR;
            }

            void setDone() && 
            {
                status_ = ExecutionStatus::COMPLETED_WITH_DONE;
            }

        private:
            friend S & tag_invoke(getScheduler_t, const ExecuteSyncReceiver & self)
            {
                return self.scheduler_;
            }

            S & scheduler_;
            ExecutionStatus & status_;
        };
    }

    inline constexpr struct executeSync_t final
    {
        template<Scheduler Sched, class S>
            requires (Sender<S> || ManySender<S>)
        ExecutionStatus operator()(Sched & scheduler, S && sender) const
        {
            auto executionStatus = ExecutionStatus::RUNNING;

            auto op = connect(
                static_cast<S&&>(sender),
                detail::ExecuteSyncReceiver<std::remove_cvref_t<Sched>>{scheduler, executionStatus});

            auto startOp = [&]() { async::start(op); };

            scheduler.post(&startOp);

            while (executionStatus == ExecutionStatus::RUNNING)
            {
                scheduler.poll();
            }

            return executionStatus;
        }
    } executeSync {};
}