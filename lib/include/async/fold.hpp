#pragma once
#include "stream.hpp"
#include "future.hpp"
#include "bind_back.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class R, class F, class T>
        class FoldOperation
        {   
            struct FoldReceiver;
            friend struct FoldReceiver;

            struct FoldReceiver
            {
                template<class T>
                void setNext(T && value) &
                {
                    // Just request the next value
                    (op_.f_)(op_.current_, static_cast<T&&>(value));
                    async::next(op_.innerOp_);
                }

                template<class E>
                void setError(E && error) &&
                {
                    async::setError(std::move(op_.receiver_), static_cast<E&&>(error));
                }

                void setDone() &&
                {
                    async::setValue(std::move(op_.receiver_), std::move(op_.current_));
                }

                FoldOperation & op_;
            };

        public:
            template<class S2, class R2, class F2, class T2>
            FoldOperation(S2 && stream, R2 && receiver, F2 && f, T2 && init)
            : innerOp_(subscribe(static_cast<S2&&>(stream), FoldReceiver(*this)))
            , receiver_(static_cast<R2&&>(receiver))
            , f_(static_cast<F2&&>(f))
            , current_(static_cast<T2&&>(init)) 
            { }

            void start()
            {
                async::start(innerOp_);
            }

        private:
            using InnerOperationType = subscribe_result_t<S, FoldReceiver>;
            InnerOperationType innerOp_;
            [[no_unique_address]] R receiver_;
            [[no_unique_address]] F f_;
            [[no_unique_address]] T current_;
        };

        template<class S, class F, class T>
            requires requires(F & f, T & t, stream_value_t<S> && v) { f(t, v); }
        class FoldSender
        {
            using Self = FoldSender<S, F, T>;
        public:
            using value_type = T;
            using error_type = stream_error_t<S>;

            template<class S2, class F2, class T2>
            FoldSender(S2 && stream, F2 && f, T2 && init)
            : stream_(static_cast<S2&&>(stream))
            , f_(static_cast<F2&&>(f))
            , init_(static_cast<T2&&>(init)) 
            { 

            }

        private:
            template<class SS, Receiver<value_type, error_type> R>
                requires std::same_as<std::remove_cvref_t<SS>, Self>
            friend auto tag_invoke(connect_t, SS && sender, R && receiver)
                -> FoldOperation<S, std::remove_cvref_t<R>, F, T>
            {
                return { 
                    static_cast<SS&&>(sender).stream_,  
                    static_cast<R&&>(receiver),
                    static_cast<SS&&>(sender).f_,
                    static_cast<SS&&>(sender).init_,
                };
            }

            [[no_unique_address]] S stream_;
            [[no_unique_address]] F f_;
            [[no_unique_address]] T init_;
        };

        template<class F, class T>
        struct FoldAdapter
        {
            template<class T2>
            void operator()(T & acc, T2 && value)
            {
                acc = f_(acc, static_cast<T2&&>(value));
            }

            template<class T2>
            void operator()(T & acc, T2 && value) const
            {
                acc = f_(acc, static_cast<T2&&>(value));
            }

            F f_;
        };
    }

    /**
     * @brief Creates a sender that waits until all items have been emitted from 
     * a stream (and it finishes with either done or an error)
     */
    inline constexpr struct foldInplace_t final
    {
        template<AnyStream S, class T, class F>
        auto operator()(S && stream, T && init, F && f) const 
            -> detail::FoldSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>, std::remove_cvref_t<T>>
        {
            return { 
                static_cast<S&&>(stream),
                static_cast<F&&>(f),
                static_cast<T&&>(init)
            };
        }

        template<class T, class F>
        auto operator()(T && init, F && f) const 
            -> BindBackResultType<foldInplace_t, std::remove_cvref_t<T>, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<T&&>(init), static_cast<F&&>(f));
        }
    } foldInplace{};

    inline constexpr struct fold_t final
    {
        template<AnyStream S, class T, class F, class FF = detail::FoldAdapter<std::remove_cvref_t<F>, std::remove_cvref_t<T>>>
            requires requires(F & f, T & t, stream_value_t<S> && v) { { f(t, v) } -> std::same_as<T>; }
        auto operator()(S && stream, T && init, F && f) const 
            -> detail::FoldSender<std::remove_cvref_t<S>, FF, std::remove_cvref_t<T>>
        {
            return { 
                static_cast<S&&>(stream),
                FF{static_cast<F&&>(f)},
                static_cast<T&&>(init)
            };
        }

        template<class T, class F>
        auto operator()(T && init, F && f) const 
            -> BindBackResultType<fold_t, std::remove_cvref_t<T>, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<T&&>(init), static_cast<F&&>(f));
        }
    } fold{};
}