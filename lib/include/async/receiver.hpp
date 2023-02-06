#pragma once
#include <type_traits>
#include <concepts>
#include "tmp/traits.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{   
    inline constexpr struct setValue_t
    {
        template<class R, class T>
        void operator()(R && r, T && value) const
        {
            static_cast<R&&>(r).setValue(static_cast<T&&>(value));
        }

        template<class R>
        void operator()(R && r) const
        {
            static_cast<R&&>(r).setValue(tmp::Void{});
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
        void operator()(R && r, S && signal) const
        {
            static_cast<R&&>(r).setNext(static_cast<S&&>(signal));
        }
    } setNext{};

    inline constexpr struct setError_t
    {
        template<class R, class E>
        void operator()(R && r, E && e) const
        {
            static_cast<R &&>(r).setError(static_cast<E&&>(e));
        }
    } setError{};

    template<typename R>
    concept AnyReceiver = 
        std::move_constructible<R> &&
        requires(std::remove_cvref_t<R> && r) {
            setDone(static_cast<R&&>(r));
        };

    template<typename R, class T, class E>
    concept Receiver = 
        AnyReceiver<R> &&
        requires(std::remove_cvref_t<R> && r, tmp::wrap_void_t<T> && value) {
            setValue(static_cast<R&&>(r), value);
        } &&
        (std::same_as<E, void> || requires(std::remove_cvref_t<R> && r, E && error) {
            setError(static_cast<R&&>(r), error);
        });

    template<typename R, class T, class E>
    concept StreamReceiver = 
        AnyReceiver<R> &&
        requires(std::remove_cvref_t<R> && r, tmp::wrap_void_t<T> && value) {
            setNext(r, value);
        } &&
        (std::same_as<E, void> || requires(std::remove_cvref_t<R> && r, E && error) {
            setError(static_cast<R&&>(r), error);
        });
}