#pragma once
#include "future.hpp"
#include "scheduler.hpp"
#include "outcome.hpp"

namespace async
{
    namespace detail
    {
        template<class T, class E>
        struct ExecutionState
        {
            // Just initialize to done for now
            Outcome<T, E> outcome = Outcome<T, E>{done};
            bool completed = false;
        };

        template<class T, class E>
        class ExecuteSyncReceiver
        {
        public:
            ExecuteSyncReceiver(ExecutionState<T, E> & status)
                : state_(status)
            {

            }

            template<class T2>
            void setValue(T2 && value) &&
            {
                state_.outcome = makeSuccess<E>(static_cast<T2&&>(value));
                state_.completed = true;
            }

            void setValue(tmp::Void &&) &&
            {
                state_.outcome = makeSuccess<E>();
                state_.completed = true;
            }

            template<class E>
            void setError(E && e) &&
            {
                state_.outcome = makeError<T>(static_cast<E&&>(e));
                state_.completed = true;
            }

            void setDone() && 
            {
                state_.outcome = makeDone<T, E>();
                state_.completed = true;
            }

        private:
            ExecutionState<T, E> & state_;
        };

        template<class S, class T, class E>
        class ExecuteSyncOnSchedulerReceiver : public ExecuteSyncReceiver<T, E>
        {
        public:
            ExecuteSyncOnSchedulerReceiver(S & scheduler, ExecutionState<T, E> & status)
            : ExecuteSyncReceiver(status), scheduler_(scheduler)
            {

            }

        private:
            friend S & tag_invoke(getScheduler_t, const ExecuteSyncOnSchedulerReceiver & self)
            {
                return self.scheduler_;
            }

            S & scheduler_;
        };
    }

    inline constexpr struct executeSync_t final
    {
        template<Scheduler Sched, AnyFuture F>
        auto operator()(Sched & scheduler, F && future) const
            -> Outcome<future_value_t<F>, future_error_t<F>>
        {
            using T = future_value_t<F>;
            using E = future_error_t<F>;

            detail::ExecutionState<T, E> executionState;

            auto operation = connect(
                static_cast<F&&>(future), detail::ExecuteSyncReceiver<T, E>{executionState}
            );

            auto startOperation = [&]() { operation.start(); };

            scheduler.post(&startOperation);

            while (!executionState.completed)
            {
                scheduler.poll();
            }

            return executionState.outcome;
        }

        template<AnyFuture S>
        constexpr auto operator()(S && future) const 
            -> Outcome<future_value_t<S>, future_error_t<S>>
        {
            using T = future_value_t<S>;
            using E = future_error_t<S>;

            detail::ExecutionState<T, E> executionState;

            auto operation = connect(
                static_cast<S&&>(future), detail::ExecuteSyncReceiver<T, E>{executionState}
            );
            operation.start();

            return executionState.outcome;
        }
    } executeSync {};
}