#pragma once
#include "sender.hpp"
#include "scheduler.hpp"
#include <tuple>

namespace async
{
    enum class CompletionStatus
    {
        COMPLETED_WITH_VALUE,
        COMPLETED_WITH_ERROR,
        COMPLETED_WITH_DONE
    };

    namespace detail
    {
        struct ExecutionState
        {
            CompletionStatus status = CompletionStatus::COMPLETED_WITH_VALUE;
            std::uint8_t numberOfCompletedTasks = 0;
        };

        template<class S>
        class ExecuteSyncReceiver
        {
        public:
            ExecuteSyncReceiver(S & scheduler, ExecutionState & status)
            : scheduler_(scheduler)
            , state_(status)
            {

            }

            void setValue() &&
            {
                state_.numberOfCompletedTasks += 1;
            }

            template<class T>
            void setSignal(T &&)
            {
                // Do nothing
            }

            template<class E>
            void setError(E && e) &&
            {
                state_.numberOfCompletedTasks += 1;
                state_.status = CompletionStatus::COMPLETED_WITH_ERROR;
            }

            void setDone() && 
            {
                state_.numberOfCompletedTasks += 1;
                if (state_.status != CompletionStatus::COMPLETED_WITH_ERROR)
                {
                    state_.status = CompletionStatus::COMPLETED_WITH_DONE;
                }
            }

        private:
            friend S & tag_invoke(getScheduler_t, const ExecuteSyncReceiver & self)
            {
                return self.scheduler_;
            }

            S & scheduler_;
            ExecutionState & state_;
        };

        template<class S, class ... Senders>
        class OperationTuple;

        template<class S, class Sender, class ... TailSenders>
        class OperationTuple<S, Sender, TailSenders...> : OperationTuple<S, TailSenders...>
        {
        public:
            template<class Sender2, class ...TailSenders2>
            OperationTuple(S & scheduler, ExecutionState & state, Sender2 && sender, TailSenders2 && ... tailSenders)
            : OperationTuple<S, TailSenders...>(scheduler, state, static_cast<TailSenders2&&>(tailSenders)...)
            , op_(async::connect(static_cast<Sender2&&>(sender), ExecuteSyncReceiver<S>{scheduler, state}))
            {

            }

            void start()
            {
                op_.start();
                OperationTuple<S, TailSenders...>::start();
            }

        private:
            ConnectResultType<Sender, ExecuteSyncReceiver<S>> op_;
        };

        template<class S>
        class OperationTuple<S>
        {
        public:
            OperationTuple(S &, ExecutionState &) 
            {

            }

            void start() { }
        };
    }

    inline constexpr struct executeSync_t final
    {
        template<Scheduler Sched, Sender ... Senders>
        CompletionStatus operator()(Sched & scheduler, Senders && ... senders) const
        {
            using OperationTupleType = detail::OperationTuple<std::remove_cvref_t<Sched>, std::remove_cvref_t<Senders>...>;

            detail::ExecutionState executionState;

            auto operationTuple = OperationTupleType(
                scheduler,
                executionState,
                static_cast<Senders&&>(senders)...
            );

            auto startOperations = [&]() 
            { 
                operationTuple.start();
            };

            scheduler.post(&startOperations);

            while (executionState.numberOfCompletedTasks < sizeof...(Senders))
            {
                scheduler.poll();
            }

            return executionState.status;
        }
    } executeSync {};
}