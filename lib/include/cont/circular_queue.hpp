#pragma once

namespace cont
{
    template<class T, int N>
    class CircularQueue
    {
    public:
        bool push(const T & value)
        {
            int nextHeadIndex = (headIndex_ + 1) % N;
            if (nextHeadIndex != tailIndex_) 
            {
                queue_[headIndex_] = value;
                headIndex_ = nextHeadIndex;
                return true;
            } 
            else 
            {
                return false;
            }
        }

        bool pop(T & value)
        {
            if (headIndex_ != tailIndex_)
            {
                value = queue_[tailIndex_];
                tailIndex_ = (tailIndex_ + 1) % N;
                return true;
            }
            return false;
        }

    private:
        T queue_[N];
        volatile int headIndex_ = 0;
        volatile int tailIndex_ = 0;
    };
}