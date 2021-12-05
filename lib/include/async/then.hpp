#pragma once
#include <type_traits>
#include <concepts>
#include "cont/box.hpp"
#include "cont/box_union.hpp"
#include "bind_back.hpp"
#include "sender.hpp"
#include <functional>
#include "tmp/type_list.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class F, class R>
        class ThenOperation
        {
            class FirstReceiver;
            template<class ... Values> class ContinuationReceiver;

            friend class FirstReceiver;
            template<class ... Values> friend class ContinuationReceiver;

            using FirstOp = ConnectResultType<S, FirstReceiver>;
            
            template<class ... Values>
            using ContinuationSenderType = tmp::CallableResultType<F, Values...>;

            template<class ... Values>
            using SecondOpType = ConnectResultType<
                ContinuationSenderType<Values...>,
                ContinuationReceiver<Values...>>;

            template<class ... Values>
            class ContinuationReceiver
            {
            public:
                template<class ... Args>
                void setValue(Args && ... args) &&
                {
                    // Copy args before calling
                    [this](auto ... values) {
                        auto & parentOp = op_;
                        parentOp.secondOp_.destruct(cont::union_t<SecondOpType<Values...>>);
                        async::setValue(std::move(parentOp.getReceiver()), static_cast<decltype(values)&&>(values)...);
                    } (static_cast<Args&&>(args)...);
                }

                template<class E>
                void setError(E && e) &&
                {
                    auto & parentOp = op_;
                    E error = e;
                    parentOp.secondOp_.destruct(cont::union_t<SecondOpType<Values...>>);
                    async::setError(std::move(parentOp.getReceiver()), std::move(e));
                }

                void setDone() &&
                {
                    auto & parentOp = op_;
                    parentOp.secondOp_.destruct(cont::union_t<SecondOpType<Values...>>);
                    async::setDone(std::move(parentOp.getReceiver()));
                }

                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const ContinuationReceiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<R>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }

                ThenOperation & op_;
            };

            class FirstReceiver
            {
            public:
                template<class ... Args> 
                void setValue(Args && ... args) &&
                {
                    // We need to copy the reference to op, the instance of this class
                    // will be destroyed when we destruct firstOp_ below
                    auto & parentOp = op_;
                    auto & continuationFactory = op_.continuationFactory_;

                    op_.firstOp_.destruct();

                    auto & nextOp = parentOp.secondOp_.constructWith([&]() {
                        return connect(
                            std::move(continuationFactory)(static_cast<Args&&>(args)...),
                            ContinuationReceiver<std::remove_cvref_t<Args>...>{parentOp});
                    });
                    async::start(nextOp);
                }

                template<class E>
                void setError(E && e) &&
                {
                    op_.firstOp_.destruct();
                    async::setError(std::move(op_.receiver_), static_cast<E&&>(e));
                }

                void setDone() &&
                {
                    op_.firstOp_.destruct();
                    async::setDone(std::move(op_.receiver_));
                }

                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const FirstReceiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<R>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }

                ThenOperation & op_;
            };
            
        public:
            template<class S2, class F2, class R2>
            ThenOperation(S2 && sender, F2 && continuationFactory, R2 && receiver)
            : receiver_(static_cast<R2&&>(receiver))
            , continuationFactory_(static_cast<F2&&>(continuationFactory))
            {
                firstOp_.constructWith([&]() {
                    return connect(
                        static_cast<S2&&>(sender), 
                        FirstReceiver{*this});
                });
            }

            ~ThenOperation()
            {
                
            }

            void start()
            {
                firstOp_.get().start();
            }

            R & getReceiver() { return receiver_; }
            const R & getReceiver() const { return receiver_; }

        private:
            [[no_unique_address]] R receiver_;
            [[no_unique_address]] F continuationFactory_;

            union {
                cont::Box<FirstOp> firstOp_;
                typename S::template value_types<cont::BoxUnion, SecondOpType> secondOp_;
            };
        };

        template<class S, class F>
        class ThenSender
        {
            template<class ... Values>
            using ContinuationSenderType = tmp::CallableResultType<F, Values...>;

            template<template <typename...> class List>
            using ContinuationSenderTypes = SenderValueTypes<S, List, ContinuationSenderType>;

            template<
                template <typename...> class Variant,
                template <typename...> class Tuple>
            struct ValueTypesImpl
            {
                template <typename... Senders>
                using apply =
                    typename tmp::unique_<
                        SenderValueTypes<Senders, tmp::TypeList, Tuple>...>::template apply<Variant>;   
            };

            template<template <typename...> class Variant>
            struct ErrorTypesImpl
            {
                template <typename... Senders>
                using apply =
                    typename tmp::unique_<
                        SenderErrorTypes<S, tmp::TypeList>,
                        SenderErrorTypes<Senders, tmp::TypeList>...>::template apply<Variant>;   
            };

        public:
            template<
                template<typename ...> typename Variant, 
                template<typename ...> typename Tuple>
            using value_types = ContinuationSenderTypes<ValueTypesImpl<Variant, Tuple>::template apply>;

            template<template<typename ...> typename Variant> 
            using error_types = ContinuationSenderTypes<ErrorTypesImpl<Variant>::template apply>;

            template<class S2, class F2>
            ThenSender(S2 && sender, F2 && continuationFactory)
            : sender_(static_cast<S2&&>(sender)), continuationFactory_(static_cast<F2&&>(continuationFactory))
            {

            }

            template<class R>
            ThenOperation<S, F, std::remove_cvref_t<R>> connect(R && r) &&
            {
                return {std::move(sender_), std::move(continuationFactory_), static_cast<R&&>(r)};
            }

        private:
            [[no_unique_address]] S sender_;
            [[no_unique_address]] F continuationFactory_;
        };
    }

    inline constexpr struct then_t
    {
        template<class S, class F>
        auto operator()(S && s, F && f) const
            -> detail::ThenSender<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(s), static_cast<F&&>(f)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<then_t, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } then;
}