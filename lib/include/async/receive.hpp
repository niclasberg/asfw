#pragma once
#include <type_traits>

namespace async
{
    namespace detail
    {
        template<class F>
        struct NextReceiver
        {
            F f;

            template<class ... Values> void setValue(Values && ...) && { }

            template<class ... Values>
            void setNext(Values && ... values)
            {
                if constexpr (sizeof...(Values) > 0)
                {
                    f(static_cast<Values&&>(values)...);
                }
                else
                {
                    f();
                }
            }

            template<class E> void setError(E &&) && { }
            void setDone() && { }
        };

        template<class F>
        struct ValueReceiver
        {
            F f;

            template<class ... Values>
            void setValue(Values && ... values) &&
            {
                if constexpr (sizeof...(Values) > 0)
                {
                    std::move(f)(static_cast<Values&&>(values)...);
                }
                else
                {
                    std::move(f)();
                }
            }

            template<class ... Values> void setNext(Values && ...) { }
            template<class E> void setError(E &&) && { }
            void setDone() && { }
        };

        template<class F>
        struct ErrorReceiver
        {
            F f;

            template<class ... Values> void setValue(Values && ...) && { }
            template<class ... Values> void setNext(Values && ...) { }

            template<class E> 
            void setError(E && e) && 
            { 
                std::move(f)(static_cast<E&&>(e));
            }

            void setDone() && { }
        };

        template<class F>
        struct DoneReceiver
        {
            F f;

            template<class ... Values> void setValue(Values && ...) { }
            template<class ... Values> void setNext(Values && ...) { }
            template<class E> void setError(E &&) &&  { }

            void setDone() && 
            { 
                std::move(f)();
            }
        };
    }
    

    template<class OnValue, class OnError>
    struct ValueAndErrorReceiver
    {
        OnValue onValue_;
        OnError onError_;

        template<class ... Values>
        void setNext(Values && ... values)
        {
            onValue_(static_cast<Values&&>(values)...);
        }

        template<class E>
        void setError(E && e) &&
        {
            OnError(static_cast<E&&>(e));
        }

        void setDone() &&
        {

        }
    };

    template<class F>
    constexpr auto receiveValue(F && f) -> detail::ValueReceiver<std::remove_cvref_t<F>>
    {
        return {static_cast<F &&>(f)};
    }

    template<class F>
    constexpr auto receiveNext(F && f) -> detail::NextReceiver<std::remove_cvref_t<F>>
    {
        return {static_cast<F &&>(f)};
    }

    template<class F>
    constexpr auto receiveError(F && f) -> detail::ErrorReceiver<std::remove_cvref_t<F>>
    {
        return {static_cast<F &&>(f)};
    }

    template<class F>
    constexpr auto receiveDone(F && f) -> detail::DoneReceiver<std::remove_cvref_t<F>>
    {
        return {static_cast<F &&>(f)};
    }
}