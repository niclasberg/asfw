#pragma once
#include <tuple>
#include "future.hpp"
#include "cont/box_union.hpp"
#include "receiver.hpp"
#include "tmp/parameter_packs.hpp"
#include "tmp/type_list.hpp"
#include "tmp/tag_invoke.hpp"
#include "util.hpp"

namespace async
{
    namespace detail
    {
        template<class Operation, std::size_t I, std::size_t NSteps>
        class SequenceReceiver
        {
        public:
            SequenceReceiver(Operation & op) : op_(op) { }

            template<class T>
            void setValue(T && value) &&
            {
                if constexpr (I < (NSteps-1))
                {
                    // Intermediate step, ignore the value and just move forward to the next
                    op_.template completeStep<I>();
                    op_.template startStep<I+1>();
                }
                else
                {
                    // Last step, propagate the received value onto the next receiver
                    // We need to grab a copy of the reference, the instance of this class
                    // will be destroyed when completeStep<NSteps-1> is called
                    Operation & op = op_;
                    op_.template completeStep<I>();
                    async::setValue(std::move(op.getReceiver()), static_cast<T&&>(value));
                }
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
            using Self = SequenceOperation<R, Senders...>;

            template<std::size_t I>
            using NthReceiverType = SequenceReceiver<Self, I, sizeof...(Senders)>;

            template<class Op, std::size_t I, std::size_t J> friend class SequenceReceiver;

            template<std::size_t I>
            using NthOperationType = connect_result_t<
                std::tuple_element_t<I, std::tuple<Senders...>>, 
                NthReceiverType<I>>;

            /**
             * @brief Start the Ith sub-operation
             * 
             * @tparam I Operation index
             */
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

            SequenceOperation(const SequenceOperation &) = delete;
            SequenceOperation & operator=(const SequenceOperation &) = delete;
            ~SequenceOperation() { }

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
            requires HasCombinableErrors<Senders...> && (sizeof...(Senders) > 0)
        class SequenceSender 
        {
            using Self = SequenceSender<Senders...>;
        public:
            using value_type = future_value_t<tmp::LastType<Senders...>>;
            using error_type = combined_future_error_t<Senders...>;

            template<class ... Senders2>
            SequenceSender(Senders2 && ... senders)
            : senders_{static_cast<Senders2&&>(senders)...}
            {

            }

        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && sender, R && receiver)
                -> SequenceOperation<std::remove_cvref_t<R>, Senders...>
            {
                return { static_cast<R&&>(receiver), static_cast<S&&>(sender).senders_ };
            }

            [[no_unique_address]] std::tuple<Senders...> senders_;
        };
    }

    /**
     * @brief Executes the senders in sequence discarding their values, finally emitting the value
     * produced by the last sender. This combinator short circuits, if any of the 
     * senders in the sequence emits an error or a done signal, the following senders
     * will not be executed.
     */
    inline constexpr struct sequence_t
    {
        template<AnyFuture ... Futures>
        auto operator()(Futures &&... futures) const
            -> detail::SequenceSender<std::remove_cvref_t<Futures>...>
        {
            return { static_cast<Futures&&>(futures)... };
        }
    } sequence{};
}