#pragma once
#include <type_traits>
#include "connect.hpp"
#include "sender_traits.hpp"
#include "receiver.hpp"

namespace async
{
    namespace _with_effect
    {
        template<class Pred, class FOnStart>
        struct Operation
        {
            Pred predecessorOperation_;
            FOnStart onStart_;

            void start()
            {
                std::move(onStart_)();
                predecessorOperation_.start();
            }
        };

        template<class InnerReceiver, class FOnCleanup>
        struct CleanupReceiver
        {   
            InnerReceiver inner_;
            FOnCleanup onCleanup_;

            template<class ... Args>
            void setValue(Args && ... values) &&
            {
                setValue(std::move(inner_), static_cast<Args &&>(values)...);
                onCleanup_();
            }

            template<class ... Args>
            void setNext(Args && ... values)
            {
                setNext(inner_, static_cast<Args &&>(values)...);
            }

            template<class E>
            void setError(E && values) &&
            {
                inner_.setError(static_cast<E &&>(values));
                onCleanup_();
            }

            void setDone() &&
            {
                inner_.setDone();
                onCleanup_();
            }
        };

        template<class ParentSender, class FOnStart>
        struct WithEffectSender
        {
            ParentSender parentSender_;
            FOnStart onStart_;

            /*template<template<typename...> typename Variant, template<typename...> typename Tuple> 
            using value_types = typename ParentSender::template value_types<Variant, Tuple>;*/

            template<class Receiver>
            auto operator()(Receiver && receiver)
            -> Operation<connect_result_t<ParentSender, helpers::remove_cvref_t<Receiver>>, FOnStart>
            {
                return {
                    async::connect(std::move(parentSender_), static_cast<Receiver &&>(receiver)), 
                    std::move(onStart_)
                };
            }
        };

        template<class ParentSender, class FOnStart, class FOnComplete>
        class WithEffectAndCleanupSender
        {
            ParentSender parentSender_;
            FOnStart onStart_;
            FOnComplete onComplete_;

        public:
            WithEffectAndCleanupSender(ParentSender && parentSender, FOnStart && onStart, FOnComplete && onComplete) 
            : parentSender_(static_cast<ParentSender &&>(parentSender))
            , onStart_(static_cast<FOnStart &&>(onStart))
            , onComplete_(static_cast<FOnComplete &&>(onComplete)) { }

            /*template<class Receiver>
            auto operator()(Receiver && receiver) const
            -> Operation<
                connect_result_t<
                    ParentSender, 
                    CleanupReceiver<helpers::remove_cvref_t<Receiver>, FOnComplete>>, 
                FOnStart>
            {
                return {
                    async::connect(std::move(parentSender_), 
                        CleanupReceiver{static_cast<Receiver &&>(receiver))};
            }*/
        };
    }

    template<class Sender, class FOnStart>
    constexpr auto withEffect(Sender && pred, FOnStart && onStart)
    -> _with_effect::WithEffectSender<helpers::remove_cvref_t<Sender>, helpers::remove_cvref_t<FOnStart>>
    {
        return {static_cast<Sender &&>(pred), static_cast<FOnStart &&>(onStart)};
    }

    template<class Sender, class FOnStart, class FOnComplete>
    constexpr auto withEffect(Sender && pred, FOnStart && onStart, FOnComplete && onComplete)
    -> _with_effect::WithEffectAndCleanupSender<
            helpers::remove_cvref_t<Sender>, 
            helpers::remove_cvref_t<FOnStart>,
            helpers::remove_cvref_t<FOnComplete>>
    {
        return _with_effect::WithEffectAndCleanupSender(
            static_cast<Sender &&>(pred), 
            static_cast<FOnStart &&>(onStart),
            static_cast<FOnComplete &&>(onComplete));
    }
}