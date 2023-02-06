#pragma once
#include <tuple>
#include "future.hpp"
#include "receiver.hpp"
#include "tmp/type_list.hpp"
#include "cont/box.hpp"
#include "util.hpp"

namespace async
{
    namespace detail
    {
        enum class WhenAllState
        {
            NOT_STARTED,
            RUNNING,
            COMPLETED_WITH_ERROR,
            COMPLETED_WITH_DONE
        };

        template<int Index, template<int> class Receiver, class ... Senders>
        class OperationTuple;

        template<int Index, template<int> class Receiver, class Sender, class ... TailSenders>
        class OperationTuple<Index, Receiver, Sender, TailSenders...>
        : public OperationTuple<Index+1, Receiver, TailSenders...>
        {
            using Super = OperationTuple<Index+1, Receiver, TailSenders...>;
        public:
            using Super::assignValue;

            template<class ParentOperation>
            OperationTuple(ParentOperation & parent, Sender && sender, TailSenders && ... tailSenders)
            : Super{parent, static_cast<TailSenders&&>(tailSenders)...}
            , op_(async::connect(static_cast<Sender&&>(sender), Receiver<Index>{parent}))
            {

            }

            void start()
            {
                op_.start();
                Super::start();
            }

            template<class T>
            void assignValue(std::integral_constant<int, Index>, T && value)
            {
                value_.construct(static_cast<T&&>(value));
            }

            template<class F, class ... Args>
            decltype(auto) unpackValues(F && f, Args && ... args)
            {
                if constexpr (std::is_void_v<future_value_t<Sender>>)
                {
                    return Super::unpackValues(static_cast<F&&>(f), static_cast<Args&&>(args)...);
                }
                else
                {
                    return Super::unpackValues(static_cast<F&&>(f), static_cast<Args&&>(args)..., value_.get());
                }
            }

            void cleanup(std::uint32_t mask)
            {
                if constexpr (!std::is_void_v<future_value_t<Sender>>)
                {
                    if ((mask & (1U << Index)) != 0)
                    {
                        value_.destruct();
                    }
                }
                Super::cleanup(mask);
            }

        private:
            [[no_unique_address]] connect_result_t<Sender, Receiver<Index>> op_;
            [[no_unique_address]] cont::Box<future_value_t<Sender>> value_;
        };

        template<int Index, template<int> class Receiver>
        class OperationTuple<Index, Receiver>
        {
        public:
            template<class ParentOperation> 
            OperationTuple(ParentOperation &) { }
            
            template<class T> 
            void assignValue(std::integral_constant<int, Index>, T &&) { }

            template<class F, class ... Args>
            decltype(auto) unpackValues(F && f, Args && ... args)
            {
                return static_cast<F&&>(f)(static_cast<Args&&>(args)...);
            }

            void cleanup(std::uint32_t) { }
            void start() { }
        };

        template<class R, class ... Senders> 
        class WhenAllOperation
        {
            using ErrorType = combined_future_error_t<Senders...>;
            inline static constexpr std::uint32_t allCompletedMask = 
                sizeof...(Senders) == 32 ? 0xFFFFFFFF : ((1U << sizeof...(Senders)) - 1U);

            template<int I> struct Receiver;
            template<int I> friend struct Receiver;

            template<int I>
            void completeSubOperation()
            {
                completedCount_ += 1;
                if (completedCount_ == sizeof...(Senders))
                {
                    if constexpr (!std::is_void_v<ErrorType>)
                    {
                        if (state_ == WhenAllState::COMPLETED_WITH_ERROR)
                        {
                            async::setError(std::move(receiver_), std::move(error_.get()));
                            error_.destruct();
                        }
                        else if (state_ == WhenAllState::COMPLETED_WITH_DONE)
                            async::setDone(std::move(receiver_));
                        else
                        {
                            async::setValue(std::move(receiver_), std::tuple<>{});
                        }
                    }
                    else
                    {
                        if (state_ == WhenAllState::COMPLETED_WITH_DONE)
                            async::setDone(std::move(receiver_));
                        else
                        {
                            async::setValue(
                                std::move(receiver_), 
                                ops_.unpackValues([]<typename ... Args>(Args && ... xs) { return std::make_tuple(static_cast<Args&&>(xs)...); }));
                        }
                    }

                    ops_.cleanup(completedMask_);
                }
            }

            template<int I>
            struct Receiver
            {
                template<class T>
                void setValue(T && value) &&
                {
                    parent_.ops_.assignValue(std::integral_constant<int, I>{}, static_cast<T&&>(value));
                    parent_.completedMask_ |= (1U << I);
                    parent_.completeSubOperation<I>(); 
                }

                void setValue(tmp::Void) &&
                {
                    parent_.completedMask_ |= (1U << I);
                    parent_.completeSubOperation<I>(); 
                }

                template<class E>
                void setError(E && error) &&
                {
                    if (parent_.state_ == WhenAllState::RUNNING)
                    {
                        parent_.state_ = WhenAllState::COMPLETED_WITH_ERROR;
                        parent_.error_.construct(static_cast<E&&>(error));
                    }

                    parent_.completeSubOperation<I>();
                }

                void setDone() &&
                {
                    if (parent_.state_ == WhenAllState::RUNNING)
                    {
                        parent_.state_ = WhenAllState::COMPLETED_WITH_DONE;
                    }
                    
                    parent_.completeSubOperation<I>();
                }

                WhenAllOperation & parent_;
            };

        public:
            WhenAllOperation(R && receiver, Senders && ... senders)
            : receiver_(static_cast<R&&>(receiver))
            , ops_(*this, static_cast<Senders&&>(senders)...)
            {

            }

            void start()
            {
                state_ = WhenAllState::RUNNING;
                ops_.start();
            }

        private:
            R receiver_;
            OperationTuple<0, Receiver, Senders...> ops_;
            [[no_unique_address]] cont::Box<tmp::wrap_void_t<ErrorType>> error_;
            int completedCount_ = 0;
            std::uint32_t completedMask_ = 0;
            WhenAllState state_ = WhenAllState::NOT_STARTED;
        };

        template<class R>
        struct MakeWhenAllOperation
        {
            R receiver_;
            template<class ... Senders>
            WhenAllOperation<R, Senders...> operator()(Senders && ... senders)
            {
                return {static_cast<R&&>(receiver_), static_cast<Senders&&>(senders)...};
            }
        };

        template<typename...Ts>
        using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

        template<AnyFuture ... Futures>
            requires HasCombinableErrors<Futures...> && (sizeof...(Futures) > 0)
        class WhenAllFuture
        {
            using Self = WhenAllFuture<Futures...>;
        public:
            using value_type = 
                tuple_cat_t<
                    std::conditional_t<
                        std::is_void_v<future_value_t<Futures>>,
                        std::tuple<>,
                        std::tuple<future_value_t<Futures>>
                    >...
                >;
            using error_type = combined_future_error_t<Futures...>;

            template<class ... Senders2>
                requires (std::is_same_v<std::remove_cvref_t<Senders2>, Futures> && ...)
            constexpr WhenAllFuture(Senders2 && ... senders)
            : senders_(static_cast<Senders2&&>(senders)...)
            {

            }

            constexpr WhenAllFuture(const WhenAllFuture &) = default;
            constexpr WhenAllFuture(WhenAllFuture &&) = default;
            constexpr ~WhenAllFuture() = default;

        private:
            template<class S, Receiver<value_type, error_type> R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && self, R && receiver) 
                -> WhenAllOperation<std::remove_cvref_t<R>, Futures...>
            {
                return std::apply(
                    MakeWhenAllOperation<std::remove_cvref_t<R>>{static_cast<R&&>(receiver)},
                    static_cast<S&&>(self).senders_);
            }

            [[no_unique_address]] std::tuple<Futures...> senders_;
        };   
    }

    /**
     * @brief Waits for all senders to complete. Accumulates
     * all values produced by the senders into a tuple. If any
     * of the senders fails with an error, or emits a done signal,
     * that signal is propagated.
     */
    constexpr inline struct whenAll_t final 
    {
        template<class ... Senders>
        constexpr auto operator()(Senders && ... senders) const
            -> detail::WhenAllFuture<std::remove_cvref_t<Senders>...> 
        {
            return {senders...};
        }
    } whenAll;
}