#pragma once
#include <concepts>

namespace async
{
    class EventEmitter;

    template<class Derived>
    class EventHandlerImpl
    {
        friend class EventEmitter;

        static void OnEvent(void * object)
        {
            static_cast<Derived *>(object)->handleEvent();
        }
    };

    template<std::invocable F>
    class EventHandler final
    {
    public:
        template<std::invocable F2>
        EventHandler(F2 && callback) 
        : callback_(static_cast<F2&&>(callback)) 
        { 

        }

    private:
        friend class EventEmitter;

        static void OnEvent(void * handlerBase)
        {
            auto * self = static_cast<EventHandler<F> *>(handlerBase);
            self->callback_();
        }

        F callback_;
    };

    template<std::invocable F>
    auto makeEventHandler(F && f) -> EventHandler<std::remove_cvref_t<F>>
    {
        return {static_cast<F&&>(f)};
    }

    class Event final
    {
    public:
        Event() : callback_(nullptr), context_(nullptr) { }
        Event(const Event &) = delete;
        Event(Event &&) = delete;
        Event & operator=(const Event &) = delete;
        Event & operator=(Event &&) = delete;

        void raise()
        {
            if (callback_)
            {
                callback_(context_);
            }
        }

        operator bool() const
        {
            return callback_ != nullptr;
        }

    private:
        void (* callback_)(void *);
        void * context_;

        friend class EventEmitter;
    };

    class EventEmitter
    {
    public:
        EventEmitter(Event * event) : event_(event) { }
        EventEmitter(const EventEmitter & rhs) { event_ = rhs.event_; }
        EventEmitter(EventEmitter && rhs) { event_ = rhs.event_; }
        EventEmitter & operator=(const EventEmitter & rhs) { event_ = rhs.event_; return *this; }
        EventEmitter & operator=(EventEmitter && rhs) { event_ = rhs.event_; return *this; }

        template<class Derived>
        bool subscribe(EventHandlerImpl<Derived> * handler)
        {
            if (event_->callback_ != nullptr)
            {
                return false;
            }

            event_->context_ = handler;
            event_->callback_ = &EventHandlerImpl<Derived>::OnEvent;
            return true;
        }

        template<std::invocable F>
        bool subscribe(EventHandler<F> * handler)
        {
            if (event_->callback_ != nullptr)
            {
                return false;
            }

            event_->context_ = handler;
            event_->callback_ = &EventHandler<F>::OnEvent;
            return true;
        }

        bool unsubscribe()
        {
            if (event_->callback_ == nullptr)
                return false;

            event_->callback_ = nullptr;
            return true;
        }

    private:
        Event * event_;
    };

    /*namespace detail
    {
        template<class R>
        class WaitOperation : EventHandlerBase
        {
        public:
            template<class R2>
            WaitOperation(Event & event, R2 && receiver)
            : Event::HandlerBase(&OnEvent)
            , event_(event)
            , receiver_(static_cast<R2&&>(receiver))
            {

            }

            void start()
            {
                if(!event_.subscribe(this))
                {
                    async::setError(receiver_, EventErrors::BUSY);
                }
            }

        private:
            static void OnEvent(Event::HandlerBase * handler)
            {
                auto & self = *static_cast<WaitOperation *>(handler);
                self.event_.unsubscribe();
                async::setValue(std::move(self.receiver_));
            }

            Event & event_;
            R receiver_;
        };

        class WaitSender
        {
        public:
            WaitSender(Event & event) : event_(event) { }

            template<class R>
            WaitOperation<R> operator()(R && r) &&
            {
                return {event_, static_cast<R&&>(r)};
            }

        private:
            Event & event_;
        };
    }

    detail::WaitSender waitFor(Event & event)
    {
        return {event};
    }

    namespace detail
    {
        template<class State, class F, class R>
        class UpdateStateWhenOperation : Event::HandlerBase
        {
        public:
            void start()
            {
                if(!event_.subscribe(this))
                {
                    async::setError(receiver_, EventErrors::BUSY);
                }
            }
            
        private:
            static void OnEvent(Event::HandlerBase * handler)
            {
                auto & self = *static_cast<UpdateStateWhenOperation *>(handler);
                
                bool shouldContinue = updateState_(state_);
                if(!shouldContinue)
                {
                    self.event_.unsubscribe();
                    async::setValue(std::move(self.receiver_));
                }
            }

            State state_;
            F updateState_;
            R receiver_;
        };

        template<class State, class F>
        class UpdateStateWhenSender
        {

        };
    }*/
}