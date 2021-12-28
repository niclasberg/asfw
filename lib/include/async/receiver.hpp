#pragma once
#include <type_traits>
#include <concepts>
#include "tmp/tag_invoke.hpp"

namespace async
{   
    inline constexpr struct setValue_t
    {
        template<class R, class ... Args>
        void operator()(R && r, Args && ... args) const
        {
            static_cast<R &&>(r).setValue(static_cast<Args&&>(args)...);
        }
    } setValue{};

    inline constexpr struct setDone_t
    {
        template<class R>
        void operator()(R && r) const
        {
            static_cast<R &&>(r).setDone();
        }
    } setDone{};

    inline constexpr struct setSignal_t
    {
        template<class R, class S>
        void operator()(R & r, S && signal) const
        {
            r.setSignal(static_cast<S&&>(signal));
        }
    } setSignal{};

    inline constexpr struct setError_t
    {
        template<class R, class E>
        void operator()(R && r, E && e) const
        {
            static_cast<R &&>(r).setError(static_cast<E&&>(e));
        }
    } setError{};

    template<typename R>
    concept Receiver = 
        std::move_constructible<R> &&
        requires(std::remove_cvref_t<R> && r) {
            setDone(static_cast<R&&>(r));
        };

    template<typename R>
    concept VoidReceiver = 
        Receiver<R> &&
        requires(std::remove_cvref_t<R> && r) {
            setValue(static_cast<R&&>(r));
        };

    template<typename R, typename... Args>
    concept ReceiverOf = 
        Receiver<R> &&
        requires(std::remove_cvref_t<R> && r, Args && ... args) {
            setValue(static_cast<R&&>(r), static_cast<Args &&>(args)...);
        };
}