#pragma once
#include <type_traits>
#include <concepts>
#include "bind_back.hpp"
#include "future.hpp"
#include <functional>
#include "tmp/traits.hpp"
#include "tmp/type_list.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class S2, class F, class R>
        class AndThenOperation
        {
            // Forward declare receivers so we can declare them as friends
            class FirstReceiver; friend class FirstReceiver;
            class SecondReceiver; friend class SecondReceiver;

            using FirstOp = connect_result_t<S, FirstReceiver>;
            using SecondOp = connect_result_t<S2, SecondReceiver>;

            class SecondReceiver
            {
            public:
                template<class T>
                void setValue(T && value) &&
                {
                    // Copy reference, this class is destroyed when op_.secondOp_ is destroyed
                    auto & op = op_;
                    op.secondOp_.~SecondOp();
                    async::setValue(std::move(op.getReceiver()), static_cast<T&&>(value));
                }

                template<class E>
                void setError(E && e) &&
                {
                    // Copy reference, this class is destroyed when op_.secondOp_ is destroyed
                    auto & op = op_;
                    E error = e;
                    op.secondOp_.~SecondOp();
                    async::setError(std::move(op.getReceiver()), std::move(e));
                }

                void setDone() &&
                {
                    // Copy reference, this class is destroyed when op_.secondOp_ is destroyed
                    auto & op = op_;
                    op.secondOp_.~SecondOp();
                    async::setDone(std::move(op.getReceiver()));
                }

                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const SecondReceiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<R>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }

                AndThenOperation & op_;
            };
            
            class FirstReceiver
            {
            public:
                template<class T> 
                void setValue(T && value) &&
                {
                    // Copy reference, this class is destroyed when op_.firstOp_ is destroyed
                    auto & op = op_;
                    op_.firstOp_.~FirstOp();

                    auto & nextOp = *std::launder(::new (reinterpret_cast<void *>(&(op.secondOp_))) 
                        SecondOp(
                            connect(
                                std::move(op.continuationFactory_)(static_cast<T&&>(value)), 
                                SecondReceiver{op})));
                    async::start(nextOp);
                }

                void setValue(tmp::Void &&) &&
                {
                    // Copy reference, this class is destroyed when op_.firstOp_ is destroyed
                    auto & op = op_;
                    op_.firstOp_.~FirstOp();

                    auto & nextOp = *std::launder(::new (reinterpret_cast<void *>(&(op.secondOp_))) 
                        SecondOp(connect((std::move(op.continuationFactory_))(), SecondReceiver{op})));
                    async::start(nextOp);
                }

                template<class E>
                void setError(E && e) &&
                {
                    // Copy reference, this class is destroyed when op_.firstOp_ is destroyed
                    auto & op = op_;
                    op_.firstOp_.~FirstOp();
                    async::setError(std::move(op.receiver_), static_cast<E&&>(e));
                }

                void setDone() &&
                {
                    // Copy reference, this class is destroyed when op_.firstOp_ is destroyed
                    auto & op = op_;
                    op_.firstOp_.~FirstOp();
                    async::setDone(std::move(op_.receiver_));
                }

                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const FirstReceiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<R>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }

                AndThenOperation & op_;
            };
            
        public:
            template<class S2, class F2, class R2>
            AndThenOperation(S2 && sender, F2 && continuationFactory, R2 && receiver)
                : receiver_(static_cast<R2&&>(receiver))
                , continuationFactory_(static_cast<F2&&>(continuationFactory))
                , firstOp_(async::connect(static_cast<S2&&>(sender), FirstReceiver{*this}))
            {
                
            }

            AndThenOperation(const AndThenOperation &) = delete;
            AndThenOperation(AndThenOperation &&) = delete;
            ~AndThenOperation() { }

            void start()
            {
                firstOp_.start();
            }

            R & getReceiver() { return receiver_; }
            const R & getReceiver() const { return receiver_; }

        private:
            [[no_unique_address]] R receiver_;
            [[no_unique_address]] F continuationFactory_;

            union {
                [[no_unique_address]] FirstOp firstOp_;
                [[no_unique_address]] SecondOp secondOp_;
            };
        };

        template<
            class SFirst, class F, 
            class SSecond = std::remove_cvref_t<tmp::CallableResultType<F, future_value_t<SFirst>>>
        > requires AnyFuture<SSecond>
        class AndThenFuture
        {
            using Self = AndThenFuture<SFirst, F, SSecond>;
        public:
            using value_type = future_value_t<SSecond>;
            using error_type = future_error_t<SSecond>;

            template<class S2, class F2>
            AndThenFuture(S2 && sender, F2 && continuationFactory)
            : sender_(static_cast<S2&&>(sender)), continuationFactory_(static_cast<F2&&>(continuationFactory))
            {

            }

        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && self, R && receiver)
                -> AndThenOperation<SFirst, SSecond, F, std::remove_cvref_t<R>>
            {
                return {
                    static_cast<S&&>(self).sender_, 
                    static_cast<S&&>(self).continuationFactory_, 
                    static_cast<R&&>(receiver)
                };
            }

            [[no_unique_address]] SFirst sender_;
            [[no_unique_address]] F continuationFactory_;
        };
    }

    /**
     * @brief Chains two asyncronous computations
     * 
     */
    inline constexpr struct andThen_t final
    {
        template<class S, class F>
        auto operator()(S && s, F && f) const
            -> detail::AndThenFuture<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return {static_cast<S&&>(s), static_cast<F&&>(f)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<andThen_t, std::remove_cvref_t<F>>
        {
            return bindBack(*this, static_cast<F&&>(f));
        }
    } andThen{};
}