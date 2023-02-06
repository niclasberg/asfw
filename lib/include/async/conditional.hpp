#pragma once
#include "future.hpp"
#include "receiver.hpp"
#include "tmp/type_list.hpp"
#include <type_traits>

namespace async
{
    namespace detail
    {
        template<AnyFuture STrue, AnyFuture SFalse, class R>
        class ConditionalOperation
        {
            struct Receiver
            {
                template<class... Values>
                void setValue(Values && ... values) &&
                {
                    auto & op = op_;
                    op.cleanUp();
                    async::setValue(std::move(op.innerReceiver_), static_cast<Values&&>(values)...);
                }

                template<class E>
                void setError(E && error) &&
                {
                    auto & op = op_;
                    op.cleanUp();
                    async::setError(std::move(op.innerReceiver_), static_cast<E&&>(error));
                }

                void setDone() &&
                {
                    auto & op = op_;
                    op.cleanUp();
                    async::setDone(std::move(op.innerReceiver_));
                }

                ConditionalOperation & op_;

            private:
                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const Receiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }
            };

            using TrueOperation = connect_result_t<STrue, Receiver>;
            using FalseOperation = connect_result_t<SFalse, Receiver>;

            void cleanUp()
            {
                if (predicateValue_)
                {
                    trueOperation_.~TrueOperation();
                }
                else
                {
                    falseOperation_.~FalseOperation();
                }
            }

        public:
            template<class STrue2, class SFalse2, class R2>
            ConditionalOperation(bool predicateValue, STrue2 && trueSender, SFalse2 && falseSender, R2 && receiver)
            : predicateValue_(predicateValue)
            , innerReceiver_(static_cast<R2&&>(receiver))
            {
                if (predicateValue_)
                {
                    new (&trueOperation_) TrueOperation{
                        async::connect(static_cast<STrue2&&>(trueSender), Receiver{*this})};
                }
                else
                {
                    new (&falseOperation_) FalseOperation{
                        async::connect(static_cast<SFalse2&&>(falseSender), Receiver{*this})};
                }
            }

            ConditionalOperation(const ConditionalOperation &) = delete;
            ConditionalOperation & operator=(const ConditionalOperation &) = delete;

            ~ConditionalOperation() { }

            void start()
            {
                if (predicateValue_)
                {
                    trueOperation_.start();
                }
                else
                {
                    falseOperation_.start();
                }
            }

            R & getReceiver() & { return innerReceiver_; }
            const R & getReceiver() const & { return innerReceiver_; }

        private:
            bool predicateValue_;
            R innerReceiver_;
            union 
            {
                TrueOperation trueOperation_;
                FalseOperation falseOperation_;
            };
        };

        template<class P, class STrue, class SFalse>
        class ConditionalSender
        {
        public:
            // The error and value type is identical for STrue and SFalse, just pick one
            using value_type = future_value_t<STrue>;
            using error_type = future_error_t<STrue>;

            template<class P2, class STrue2, class SFalse2>
            ConditionalSender(P2 && predicate, STrue2 && senderIfTrue, SFalse2 && senderIfFalse)
            : predicate_(static_cast<P2&&>(predicate))
            , senderIfTrue_(static_cast<STrue2&&>(senderIfTrue))
            , senderIfFalse_(static_cast<SFalse2&&>(senderIfFalse))
            {

            }

            template<class R>
            auto connect(R && receiver) && -> ConditionalOperation<STrue, SFalse, std::remove_cvref_t<R>>
            {
                return {std::move(predicate_)(), std::move(senderIfTrue_), std::move(senderIfFalse_), static_cast<R&&>(receiver)};
            }

        private:
            P predicate_;
            STrue senderIfTrue_;
            SFalse senderIfFalse_;
        };

        struct ConstPredicate
        {
            bool operator()() const
            {
                return value_;
            }

            const bool value_;
        };
    }

    inline constexpr struct conditional_t
    {
        template<std::predicate F, AnyFuture STrue, AnyFuture SFalse>
            requires 
                std::same_as<future_value_t<STrue>, future_value_t<STrue>> &&
                std::same_as<future_error_t<STrue>, future_error_t<STrue>>
        auto operator()(F && predicate, STrue && senderIfTrue, SFalse && senderIfFalse) const
            -> detail::ConditionalSender<std::remove_cvref_t<F>, std::remove_cvref_t<STrue>, std::remove_cvref_t<SFalse>>
        {
            return {static_cast<F&&>(predicate), static_cast<STrue&&>(senderIfTrue), static_cast<SFalse&&>(senderIfFalse)};
        }

        template<AnySender STrue, AnySender SFalse>
            requires 
                std::same_as<sender_value_t<STrue>, sender_value_t<STrue>> &&
                std::same_as<sender_error_t<STrue>, sender_error_t<STrue>>
        auto operator()(bool value, STrue && senderIfTrue, SFalse && senderIfFalse) const
            -> detail::ConditionalSender<detail::ConstPredicate, std::remove_cvref_t<STrue>, std::remove_cvref_t<SFalse>>
        {
            return {detail::ConstPredicate{value}, static_cast<STrue&&>(senderIfTrue), static_cast<SFalse&&>(senderIfFalse)};
        }

    } conditional {};
}