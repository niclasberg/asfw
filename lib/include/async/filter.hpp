#pragma once
#include "stream.hpp"
#include "receiver.hpp"
#include <concepts>

namespace async
{
    namespace detail
    {
        template<class P, class R>
        class FilterReceiver
        {
        public:
            template<class P2, class R2>
            FilterReceiver(P2 && predicate, R2 && receiver)
            : predicate_(static_cast<P2&&>(predicate))
            , receiver_(static_cast<R2&&>(receiver))
            {

            }

            template<class T>
            void setNext(T && value) &
            {
                if (predicate_(value))
                {
                    async::setSignal(receiver_, static_cast<T&&>(signal));
                }
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(receiver_), static_cast<E&&>(e));
            }

            void setDone() &&
            {
                async::setDone(std::move(receiver_));
            }

        private:
            P predicate_;
            R receiver_;
        };

        template<AnyStream S, class P>
        class FilterStream
        {
        public:
            using value_type = stream_value_t<S>;
            using error_type = stream_error_t<S>;

            template<class R, class ReceiverType = std::remove_cvref_t<R>>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, FilterReceiver<P, ReceiverType>>
            {
                return async::connect(
                    std::move(sender_),
                    FilterReceiver<P, ReceiverType>{std::move(predicate_), static_cast<R&&>(receiver)});
            }

        private:
            S sender_;
            P predicate_;
        };
    }

    inline constexpr struct filter_t final
    {
        template<Sender S, class P>
        auto operator()(S && sender, P && predicate) const
            -> detail::FilterStream<std::remove_cvref_t<S>, std::remove_cvref_t<P>>
        {
            return {static_cast<S&&>(sender), static_cast<P&&>(predicate)};
        }

        template<class P>
        auto operator()(P && f) const -> BindBackResultType<filter_t, P>
        {
            return bindBack(*this, static_cast<P &&>(f));
        }
    } filter {};
}