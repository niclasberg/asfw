#pragma once
#include <type_traits>
#include "sender.hpp"
#include "receiver.hpp"
#include "bind_back.hpp"
#include "tmp/type_list.hpp"
#include "tmp/traits.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class R, class F>
        class TransformReceiver
        {
        public:
            template<class R2, class F2>
            TransformReceiver(R2 && receiver, F2 && f)
            : receiver_(static_cast<R2&&>(receiver))
            , f_(static_cast<F2&&>(f))
            {

            }

            template<class ... Values>
            void setValue(Values && ... values) &&
            {
                using Result = std::invoke_result_t<F, Values...>;
                if constexpr (std::is_void_v<Result>)
                {
                    f_(static_cast<Values &&>(values)...);
                    async::setValue(std::move(receiver_));
                }
                else
                {   
                    async::setValue(std::move(receiver_), f_(static_cast<Values &&>(values)...));
                }
            }

            template<class T>
            void setSignal(T && signal)
            {
                async::setSignal(receiver_, static_cast<T&&>(signal));
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(receiver_), static_cast<E &&>(e));
            }

            void setDone() &&
            {
                async::setDone(std::move(receiver_));
            }

        private:
            template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, const TransformReceiver & self, Args &&... args) 
                -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
            {
                return cpo(self.receiver_, static_cast<Args&&>(args)...);
            }

            R receiver_;
            F f_;
        };

        template<class T>
        using VoidFix = tmp::ConditionalType<std::is_void_v<T>, tmp::TypeList<>, tmp::TypeList<T>>;

        template<class ParentSender, class F>
        class TransformSender
        {
            template <typename... Args>
            using Result = tmp::TypeList<VoidFix<std::invoke_result_t<F, Args...>>>;

        public:
            template<template<typename...> typename Variant, template<typename...> typename Tuple>
            using value_types = tmp::nestedApply_<
                    SenderValueTypes<ParentSender, tmp::unique_, Result>,
                    Variant, Tuple>;

            template<template<typename...> typename Variant>
            using error_types = SenderErrorTypes<ParentSender, Variant>;

            template<template<typename...> typename Variant>
            using signal_types = SenderSignalTypes<ParentSender, Variant>;

            template<class F2>
            TransformSender(ParentSender && parentSender, F2 && f)
            : parentSender_(static_cast<ParentSender &&>(parentSender))
            , f_(static_cast<F2 &&>(f))
            {

            }

            template<Receiver R>
            auto connect(R && receiver) &&
            {
                return async::connect(
                    std::move(parentSender_), 
                    TransformReceiver<std::remove_cvref_t<R>, F>{static_cast<R &&>(receiver), std::move(f_)});
            }

            template<Receiver R>
            auto connect(R && receiver) const &
            {
                return async::connect(
                    parentSender_, 
                    TransformReceiver<std::remove_cvref_t<R>, F>{static_cast<R &&>(receiver), f_});
            }

        private:
            ParentSender parentSender_;
            F f_;
        };
    }

    inline constexpr struct transform_t 
    {
        template<Sender S, class F>
        auto operator()(S && parent, F && f) const
            -> detail::TransformSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S &&>(parent), static_cast<F &&>(f)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<transform_t, F>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } transform {};
}