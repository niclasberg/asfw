#pragma once
#include <algorithm>
#include <tuple>
#include "sender.hpp"
#include "cont/box_union.hpp"
#include "receiver.hpp"
#include "tmp/parameter_packs.hpp"
#include "tmp/type_list.hpp"
#include "tmp/tag_invoke.hpp"
#include "util.hpp"
#include <utility>

namespace async
{
    namespace detail
    {
        template<class Operation, std::size_t I, std::size_t NSteps>
        class SequenceReceiver
        {
        public:
            SequenceReceiver(Operation & op) : op_(op) { }

            template<class ... Values>
            void setValue(Values && ... values) &&
            {
                if constexpr (I == (NSteps-1))
                {
                    // Need to grab a copy of the reference, the instance of this class
                    // will be destroyed when completeStep<NSteps-1> is called
                    Operation & op = op_;
                    op_.template completeStep<I>();
                    async::setValue(std::move(op.getReceiver()), static_cast<Values&&>(values)...);
                }
                else
                {
                    op_.template completeStep<I>();
                    op_.template startStep<I+1>();
                }
            }

            template<class S>
            void setSignal(S && signal)
            {
                async::setSignal(op_.getReceiver(), static_cast<S&&>(signal));
            }

            template<class E>
            void setError(E && e) &&
            {
                Operation & op = op_;
                op.template completeStep<I>();
                async::setError(std::move(op.getReceiver()), static_cast<E&&>(e));
            }

            void setDone() &&
            {
                Operation & op = op_;
                op.template completeStep<I>();
                async::setDone(std::move(op.getReceiver()));
            }

        private:
            template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, const SequenceReceiver<Operation, I, NSteps> & self, Args &&... args) 
                -> decltype(cpo(std::declval<const typename Operation::ReceiverType &>(), static_cast<Args&&>(args)...))
            {
                return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
            }

            Operation & op_;
        };

        template<class R, class ... Senders>
            requires (sizeof...(Senders) > 0)
        class SequenceOperation
        {
            template<std::size_t I>
            using NthReceiverType = SequenceReceiver<SequenceOperation<R, Senders...>, I, sizeof...(Senders)>;

            template<class Op, std::size_t I, std::size_t J> friend class SequenceReceiver;

            template<std::size_t I>
            using NthOperationType = ConnectResultType<
                std::tuple_element_t<I, std::tuple<Senders...>>, 
                NthReceiverType<I>>;

            template<std::size_t I>
            void startStep()
            {
                auto & nextOp = operationVariant_.constructWith([this]() {
                    return connect(
                        std::move(std::get<I>(senders_)), 
                        NthReceiverType<I>{*this});
                });
                async::start(nextOp);
            }

            template<std::size_t I>
            void completeStep()
            {
                operationVariant_.destruct(cont::union_t<NthOperationType<I>>);
            }

        public:
            using ReceiverType = R;

            template<class R2>
            SequenceOperation(R2 && receiver, std::tuple<Senders...> && senders)
            : receiver_(static_cast<R2&&>(receiver))
            , senders_(std::move(senders))
            {
            }

            template<class R2>
            SequenceOperation(R2 && receiver, const std::tuple<Senders...> & senders)
            : receiver_(static_cast<R2&&>(receiver))
            , senders_(senders)
            {
            }

            ~SequenceOperation() { }
            
            SequenceOperation(const SequenceOperation &) = delete;
            SequenceOperation & operator=(const SequenceOperation &) = delete;

            void start()
            {
                startStep<0>();
            }

            R & getReceiver() { return receiver_; }
            const R & getReceiver() const { return receiver_; }

        private:
            template<std::size_t ... I>
            inline static auto makeOperationUnionHelper(std::index_sequence<I...>)
            {
                return cont::BoxUnion<NthOperationType<I>...>{};
            }
            using UnionType = decltype(makeOperationUnionHelper(std::make_index_sequence<sizeof...(Senders)>{}));

            [[no_unique_address]] R receiver_;
            [[no_unique_address]] std::tuple<Senders...> senders_;
            [[no_unique_address]] UnionType operationVariant_;
        };

        template<class ... Senders>
        struct SequenceSender 
        {
            template<
                template<typename ...> typename Variant, 
                template<typename ...> typename Tuple>
            using value_types = typename tmp::LastType<Senders...>::template value_types<Variant, Tuple>;

            template<template<typename ...> typename Variant>
            using error_types = CombinedSenderErrorTypes<Variant, Senders...>;

            template<template<typename...> typename Variant>
            using signal_types = CombinedSenderSignalTypes<Variant, Senders...>;

            template<class ... Senders2>
            SequenceSender(Senders2 && ... senders)
            : senders_{static_cast<Senders2&&>(senders)...}
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> SequenceOperation<std::remove_cvref_t<R>, Senders...>
            {
                return {static_cast<R&&>(receiver), std::move(senders_)};
            }

            template<class R>
            auto connect(R && receiver) const &
                -> SequenceOperation<std::remove_cvref_t<R>, Senders...>
            {
                return {static_cast<R&&>(receiver), senders_};
            }

            [[no_unique_address]] std::tuple<Senders...> senders_;
        };
    }

    inline constexpr struct sequence_t
    {
        template<Sender ... Senders>
        auto operator()(Senders &&... senders) const
            -> detail::SequenceSender<std::remove_cvref_t<Senders>...>
        {
            return { static_cast<Senders&&>(senders)... };
        }
    } sequence{};
}