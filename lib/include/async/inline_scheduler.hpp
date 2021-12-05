#pragma once
#include "scheduler.hpp"
#include "delegate.hpp"

namespace async
{
    class InlineScheduler
    {
        /*template<class S, class R>
        struct Operation
        {
            void start()
            {
                
            }
        };

        struct Sender
        {
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = Variant<Tuple<>>;
            template<template<typename...> class Variant> 
            using error_types = Variant<>;

        };*/

    public:

        bool post(Delegate<void(void)> action)
        {
            // Evaluate eagerly
            action();
            return true;
        }

        bool postFromISR(Delegate<void(void)> action)
        {
            action();
            return true;
        }

        void poll() { }
    };
}