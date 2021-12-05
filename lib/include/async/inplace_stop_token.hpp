#pragma once
#include <concepts>
#include "stop_token.hpp"

namespace async
{
    class InplaceStopToken;
    class InplaceStopSource;
    template<std::invocable F> class InplaceStopCallback;

    class InplaceStopCallbackBase
    {
    public:
        virtual void execute() = 0;

    private:
        friend class InplaceStopSource;
        InplaceStopCallbackBase * next_ = nullptr;
    };

    class InplaceStopSource
    {
    public:
        InplaceStopSource() = default;

        InplaceStopSource(const InplaceStopSource &) = delete;
        InplaceStopSource(InplaceStopSource &&) = delete;
        InplaceStopSource & operator=(const InplaceStopSource &) = delete;
        InplaceStopSource & operator=(InplaceStopSource &&) = delete;

        void requestStop()
        {
            // Loop through and call all callbacks
            stopRequested_ = true;
            while(stopCallbackList_ != nullptr)
            {
                stopCallbackList_->execute();
                stopCallbackList_ = stopCallbackList_->next_;
            }
        }

        bool stopRequested() const
        {
            return stopRequested_;
        }

        InplaceStopToken getToken();

    private:
        template<std::invocable F> 
        friend class InplaceStopCallback;

        bool tryAddCallback(InplaceStopCallbackBase * stopCallback)
        {
            if (stopRequested_)
                return false;
            stopCallback->next_ = stopCallbackList_;
            stopCallbackList_ = stopCallback;
            return true;
        }

        void removeCallback(InplaceStopCallbackBase * stopCallback)
        {
            if(stopCallback == stopCallbackList_)
            {
                stopCallbackList_ = stopCallbackList_->next_;
            }
            else
            {
                for (auto * it = stopCallbackList_; it != nullptr; it = it->next_)
                {
                    if (it->next_ == stopCallback)
                    {

                    }
                }
            }
        }

        bool stopRequested_ = false;
        InplaceStopCallbackBase * stopCallbackList_ = nullptr;
    };

    class InplaceStopToken
    {
    public:
        template<class F>
        using CallbackType = InplaceStopCallback<F>;

        bool stopRequested() const 
        {
            return source_ && source_->stopRequested();
        }

        bool stopPossible() const
        {
            return source_;
        }

    private:
        friend class InplaceStopSource;
        template<std::invocable F> 
        friend class InplaceStopCallback;

        InplaceStopToken(InplaceStopSource * source) : source_(source) { }

        InplaceStopSource * source_;
    };

    inline InplaceStopToken InplaceStopSource::getToken()
    {
        return {this};
    }

    template<std::invocable F>
    class InplaceStopCallback : InplaceStopCallbackBase
    {
    public:
        InplaceStopCallback(InplaceStopToken & token, F && f)
        : source_(token.source_)
        , f(static_cast<F&&>(f))
        {
            if (source_ != nullptr)
            {
                if(!source_->tryAddCallback(this))
                {
                    source_ = nullptr;
                    // Source is stopped already
                    this->execute();
                }
            }
        }

        ~InplaceStopCallback()
        {
            if (source_ != nullptr)
            {
                source_->removeCallback(this);
            }
        }

        void execute() override
        {
            std::move(f)();
        }

    private:
        InplaceStopSource * source_;
        F f;
    };
}