#pragma once
#include "types.hpp"
#include "delegate.hpp"
#include "cont/circular_queue.hpp"
#include "async/event.hpp"
#include "board/interrupts.hpp"
#include <array>

namespace schedulers
{
    template<std::uint32_t NTasks, std::uint32_t NTimers, class InterruptController, std::uint32_t ticksPerMs = 1U>
    class CooperativeScheduler : async::EventHandlerImpl<CooperativeScheduler<NTasks, NTimers, InterruptController, ticksPerMs>>
    {
        using TimedFunctionType = Delegate<int(void)>;
        struct FutureTask
        {
            TimedFunctionType func;
            std::uint32_t executeAtTick;
        };

    public:
        using FunctionType = Delegate<void(void)>;

        CooperativeScheduler(async::EventEmitter timerEvent)
        : timerEvent_(timerEvent)
        {
            timerEvent_.subscribe(this);
        }

        ~CooperativeScheduler()
        {
            timerEvent_.unsubscribe();
        }

        CooperativeScheduler(const CooperativeScheduler &) = delete;
        CooperativeScheduler(CooperativeScheduler &&) = delete;
        CooperativeScheduler & operator=(const CooperativeScheduler &) = delete;
        CooperativeScheduler & operator=(CooperativeScheduler &&) = delete;

        bool postFromISR(FunctionType func)
        {
            return postImpl(func);
        }

        bool post(FunctionType func)
        {
            InterruptController::disableIRQs();
            bool ret = postImpl(func);
            InterruptController::enableIRQs();
            return ret;
        }

        void poll()
        {
            // Run timed tasks first
            auto currentTick = currentTick_;
            for (auto & task : futureTasks_)
            {
                if (task.func && currentTick >= task.executeAtTick)
                {
                    int delayUntilNext = task.func();
                    if (delayUntilNext > 0)
                    {
                        task.executeAtTick = currentTick + delayUntilNext * ticksPerMs;
                    }
                    else
                    {
                        task.func.reset();
                    }
                }
            }

            // Run enqueued tasks
            if (headIndex_ != tailIndex_)
            {
                queue_[tailIndex_]();
                tailIndex_ = (tailIndex_ + 1) % NTasks;
            }
        }

        bool postAfter(std::uint32_t delayMs, TimedFunctionType delegate)
        {
            // Assign to the first unassigned slot
            for (auto & futureTask : futureTasks_)
            {
                if (!futureTask.func)
                {
                    futureTask.executeAtTick = currentTick_ + delayMs * ticksPerMs;
                    futureTask.func = delegate;
                    return true;
                }
            }
            return false;
        }

        void handleEvent()
        {
            currentTick_ = currentTick_ + 1;
        }

    private:
        bool postImpl(FunctionType func)
        {
            std::uint_fast32_t nextHeadIndex = (headIndex_ + 1) % NTasks;
            if (nextHeadIndex != tailIndex_) 
            {
                queue_[headIndex_] = func;
                headIndex_ = nextHeadIndex;
                return true;
            } 
            else 
            {
                return false;
            }
        }

        FunctionType queue_[NTasks];
        volatile std::uint_fast32_t headIndex_ = 0;
        volatile std::uint_fast32_t tailIndex_ = 0;
        std::array<FutureTask, NTimers> futureTasks_ = {};
        volatile std::uint32_t currentTick_ = 0;
        async::EventEmitter timerEvent_;
    };

    template<class Board>
    auto makeCooperativeScheduler(Board board)
        -> CooperativeScheduler<16, 1, typename Board::InterruptController>
    {
        // Enable SysTick interrupt with a frequency of 1 ms
        board.enableSysTickIRQ(uint32_c<1'000>);

        return {board.getInterruptEvent(board::Interrupts::SysTick)};
    }
}