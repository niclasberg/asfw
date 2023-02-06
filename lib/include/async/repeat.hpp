#pragma once
#include "sender.hpp"
#include "cont/box.hpp"
#include "receiver.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class R, class P>
        class RepeatOperation
        {   
            class RepeatReceiver
            {
            public:
                RepeatReceiver(RepeatOperation & op) : op_(op) { }

                template<class ... Values>
                void setValue(Values && ... values) &&
                {
                    if (op_.predicate_(static_cast<Values&&>(values)...))
                    {
                        auto & op = op_;
                        op.cleanup();
                        async::setValue(std::move(op.getReceiver()));
                    }
                    else
                    {
                        op_.restart();
                    }
                }

                template<class E>
                void setError(E && e) && 
                {
                    auto & op = op_;
                    op.cleanup();
                    async::setError(std::move(op.getReceiver()), static_cast<E&&>(e));
                }

                void setDone() && 
                {
                    auto & op = op_;
                    op.cleanup();
                    async::setDone(std::move(op.getReceiver()));
                }

            private:
                template<class Cpo, class ... Args>
                friend auto tag_invoke(Cpo cpo, const RepeatReceiver & self, Args &&... args) 
                    -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
                {
                    return cpo(self.op_.getReceiver(), static_cast<Args&&>(args)...);
                }

                RepeatOperation & op_;
            };

            using InnerOperation = connect_result_t<S, RepeatReceiver>;

        public:
            template<class S2, class R2, class P2>
            RepeatOperation(S2 && sender, R2 && receiver, P2 && predicate)
                : sender_(static_cast<S2&&>(sender))
                , receiver_(static_cast<R2&&>(receiver))
                , predicate_(static_cast<P2&&>(predicate))
            {
                innerOperationStorage_.constructWith([this]() {
                    return async::connect(sender_, RepeatReceiver{*this});
                });
            }

            RepeatOperation(const RepeatOperation &) = delete;
            RepeatOperation & operator=(const RepeatOperation &) = delete;
            ~RepeatOperation() { }

            void start()
            {
                async::start(innerOperationStorage_.get());
            }

            R & getReceiver() & { return receiver_; }
            const R & getReceiver() const & { return receiver_; }

        private:
            void cleanup()
            {
                innerOperationStorage_.destruct();
            }

            void restart()
            {
                innerOperationStorage_.destruct();
                auto & op = innerOperationStorage_.constructWith([this]() {
                    return async::connect(sender_, RepeatReceiver{*this});
                });
                async::start(op);
            }

            [[no_unique_address]] S sender_;
            [[no_unique_address]] R receiver_;
            [[no_unique_address]] P predicate_;

            cont::Box<InnerOperation> innerOperationStorage_;
        };

        template<class S, class P>
        class RepeatSender
        {
        public:
            using value_type = void;
            using error_type = future_error_t<S>;

            template<class S2, class P2>
            RepeatSender(S2 && sender, P2 && predicate) 
                : sender_(static_cast<S2&&>(sender)) 
                , predicate_(static_cast<P2&&>(predicate))
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> RepeatOperation<S, std::remove_cvref_t<R>, P>
            {
                return {std::move(sender_), static_cast<R&&>(receiver), std::move(predicate_)};
            }

        private:
            S sender_;
            P predicate_;
        };

        struct AlwaysFalse
        {
            bool operator()() const
            {
                return false;
            }
        };
    }

    inline constexpr struct repeatUntil_t final
    {
        template<Sender S, class P>
        auto operator()(S && sender, P && predicate) const
            -> detail::RepeatSender<std::remove_cvref_t<S>, std::remove_cvref_t<P>>
        {
            return {static_cast<S&&>(sender), static_cast<P&&>(predicate)};
        }
    } repeatUntil{};

    inline constexpr struct repeat_t final
    {
        template<Sender S>
        auto operator()(S && sender) const
            -> detail::RepeatSender<std::remove_cvref_t<S>, detail::AlwaysFalse>
        {
            return {static_cast<S&&>(sender), detail::AlwaysFalse{}};
        }
    } repeat{};
}