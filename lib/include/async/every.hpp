#pragma once
#include "receiver.hpp"
#include "scheduler.hpp"
#include "delegate.hpp"

namespace async
{
    namespace detail
    {
        template<class R, TimedScheduler Sched>
        class EveryOperation
        {
        public:
            template<class R2>
            EveryOperation(R2 && receiver, Sched & scheduler, std::uint32_t interval)
            : receiver_(static_cast<R2&&>(receiver))
            , scheduler_(scheduler)
            , interval_(interval)
            {

            }

            void start()
            {
                scheduler_.postAfter(interval_, {memFn<&EveryOperation::onTick>, *this});
            }

        private:
            int onTick()
            {
                async::setNext(receiver_);
                return interval_;
            }

            R receiver_;
            Sched & scheduler_;
            std::uint32_t interval_;
        };

        struct EverySender
        {    
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using next_types = Variant<Tuple<>>;

            template<template<typename...> class Variant>
            using error_types = Variant<>;

            template<class R>
            auto connect(R && receiver) &&
                -> EveryOperation<std::remove_cvref_t<R>, ReceiverSchedulerType<R>>
            {
                return {static_cast<R&&>(receiver), getScheduler(receiver), interval_};
            }

            std::uint32_t interval_;
        };
    }

    inline constexpr struct every_t final
    {
        detail::EverySender operator()(std::uint32_t intervalMs) const
        {
            return {intervalMs};
        }
    } every{};
}