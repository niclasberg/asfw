#pragma once
#include "tmp/tag_invoke.hpp"
#include "scheduler.hpp"
#include "sender.hpp"
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

            template<class ... Values>
                requires ReceiverOf<R, Values...>
            void setValue(Values && ... values) &&
            {
                async::setValue(std::move(innerReceiver_), static_cast<Values&&>(values)...);
            }

            template<class ... Values>
                requires ManyReceiverOf<R, Values...>
            void setNext(Values && ... values)
            {
                async::setNext(innerReceiver_, static_cast<Values&&>(values)...);
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
            /*template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, Self && self, Args &&... args) 
                -> decltype(cpo(static_cast<Self&&>(self).innerReceiver_, static_cast<Args&&>(args)...))
            {
                return cpo(static_cast<Self&&>(self).innerReceiver_, static_cast<Args&&>(args)...);
            }*/

            friend S & tag_invoke(getScheduler_t, const UseSchedulerReceiver & self)
            {
                return self.scheduler_;
            }

            R innerReceiver_; 
            S & scheduler_;
        };

        template<Scheduler S, Sender ParentSender>
        class UseSchedulerSender
        {
        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = SenderValueTypes<ParentSender, Variant, Tuple>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<ParentSender, Variant>;

            template<class ParentSender2>
            UseSchedulerSender(S & scheduler, ParentSender2 && parentSender) 
            : parentSender_(static_cast<ParentSender2&&>(parentSender))
            , scheduler_(scheduler) 
            { 

            }

            template<class R>
            auto connect(R && receiver) &&
                -> ConnectResultType<ParentSender, UseSchedulerReceiver<S, std::remove_cvref_t<R>>>
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
        template<Scheduler S, Sender ParentSender>
        auto operator()(S & scheduler, ParentSender && parentSender) const
            -> detail::UseSchedulerSender<std::remove_cvref_t<S>, std::remove_cvref_t<ParentSender>>
        {
            return {scheduler, static_cast<ParentSender&&>(parentSender)};
        }
    } useScheduler{};
}