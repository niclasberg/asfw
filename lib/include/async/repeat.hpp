#pragma once
#include "sender.hpp"
#include "cont/box.hpp"
#include "receiver.hpp"

namespace async
{
    namespace detail
    {
        template<class S, class R>
        class RepeatOperation
        {   
            class RepeatReceiver
            {
            public:
                RepeatReceiver(RepeatOperation & op) : op_(op) { }

                void setValue() &&
                {
                    op_.restart();
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

            using InnerOperation = ConnectResultType<S, RepeatReceiver>;

        public:
            template<class S2, class R2>
            RepeatOperation(S2 && sender, R2 && receiver)
            : sender_(static_cast<S2&&>(sender))
            , receiver_(static_cast<R2&&>(receiver))
            {
                innerOperationStorage_.constructWith([this]() {
                    return async::connect(sender_, RepeatReceiver{*this});
                });
            }

            RepeatOperation(const RepeatOperation &) = delete;
            RepeatOperation & operator=(const RepeatOperation &) = delete;

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

            S sender_;
            R receiver_;
            cont::Box<InnerOperation> innerOperationStorage_;
        };

        template<class S>
        class RepeatSender
        {
        public:
            template<template<typename...> class Variant, template<typename...> class Tuple>
            using value_types = Variant<Tuple<>>;

            template<template<typename...> class Variant>
            using error_types = SenderErrorTypes<S, Variant>;

            template<class S2>
            RepeatSender(S2 && sender) : sender_(static_cast<S2&&>(sender)) 
            {

            }

            template<class R>
            auto connect(R && receiver) &&
                -> RepeatOperation<S, std::remove_cvref_t<R>>
            {
                return {std::move(sender_), static_cast<R&&>(receiver)};
            }

        private:
            S sender_;
        };
    }

    inline constexpr struct repeat_t final
    {
        template<Sender S>
        auto operator()(S && sender) const
            -> detail::RepeatSender<std::remove_cvref_t<S>>
        {
            return {static_cast<S&&>(sender)};
        }
    } repeat{};
}