#pragma once
#include <tuple>
#include "inplace_stop_token.hpp"
#include "sender.hpp"
#include "receiver.hpp"
#include "tmp/type_list.hpp"

namespace async
{
    namespace detail
    {
        template<int Index, template<int> class Receiver, class ... Senders>
        class OperationTuple;

        template<int Index, 
            template<int> class Receiver,
            class Sender, 
            class ... TailSenders>
        class OperationTuple<Index, Receiver, Sender, TailSenders...>
        : OperationTuple<Index+1, Receiver, TailSenders...>
        {
        public:
            template<class ParentOperation>
            OperationTuple(ParentOperation & parent, Sender && sender, TailSenders && ... tailSenders)
            : OperationTuple<Index+1, Receiver, TailSenders...>{parent, static_cast<TailSenders&&>(tailSenders)...}
            , op_(async::connect(static_cast<Sender&&>(sender), Receiver<Index>{parent}))
            {

            }

            void start()
            {
                op_.start();
                OperationTuple<Index+1, Receiver, TailSenders...>::start();
            }

        private:
            ConnectResultType<Sender, Receiver<Index>> op_;
        };

        template<int Index, template<int> class Receiver>
        class OperationTuple<Index, Receiver>
        {
        public:
            template<class ParentOperation>
            OperationTuple(ParentOperation &) 
            {

            }

            void start() { }
        };

        template<class R, class ... Senders> 
        class WhenAllOperation
        {
            template<int I> struct Receiver;
            template<int I> friend struct Receiver;

            template<int I>
            void completeSubOperation()
            {
                completedCount_ += 1;
                if (completedCount_ == sizeof...(Senders))
                {
                    if (receivedDone_)
                    {
                        async::setDone(std::move(receiver_));
                    }
                    else
                    {
                        async::setValue(std::move(receiver_));
                    }
                }
            }

            template<int I>
            struct Receiver
            {
                void setValue() &&
                {
                    parent_.completeSubOperation<I>(); 
                }

                template<class E>
                void setError(E &&) &&
                {
                    if ( ! parent_.receivedDone_)
                    {
                        parent_.receivedDone_ = true;
                    }

                    parent_.completeSubOperation<I>();
                }

                void setDone() &&
                {
                    if ( ! parent_.receivedDone_)
                    {
                        parent_.receivedDone_ = true;
                        parent_.stopSource_.requestStop();
                    }
                    
                    parent_.completeSubOperation<I>();
                }

                InplaceStopToken getStopToken() const
                {
                    return parent_.stopSource_.getToken();
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
                ops_.start();
            }

        private:
            R receiver_;
            OperationTuple<0, Receiver, Senders...> ops_;  
            InplaceStopSource stopSource_;
            int completedCount_ = 0;
            bool receivedDone_ = false;
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

        template<class ... Senders>
        class WhenAllSender
        {
        public:
            template<
                template<typename ...> typename Variant, 
                template<typename ...> typename Tuple>
            using value_types = Variant<Tuple<>>;

            template<template<typename ...> typename Variant>
            using signal_types = typename tmp::unique_<tmp::concat_<SenderSignalTypes<Senders, tmp::TypeList>...>>::template apply<Variant>;

            template<template<typename ...> typename Variant>
            using error_types = typename tmp::unique_<tmp::concat_<SenderErrorTypes<Senders, tmp::TypeList>...>>::template apply<Variant>;

            template<class ... Senders2>
            WhenAllSender(Senders2 && ... senders)
            : senders_(static_cast<Senders2&&>(senders)...)
            {

            }

            template<VoidReceiver R>
            WhenAllOperation<std::remove_cvref_t<R>, Senders...> connect(R && receiver) &&
            {
                return std::apply(
                    MakeWhenAllOperation<std::remove_cvref_t<R>>{static_cast<R&&>(receiver)},
                    std::move(*this).senders_);
            }

        private:
            std::tuple<Senders...> senders_;
        };   
    }

    constexpr inline struct whenAll_t {
        template<class ... Senders>
        auto operator()(Senders && ... senders) const
            -> detail::WhenAllSender<std::remove_cvref_t<Senders>...> 
        {
            return {senders...};
        }
    } whenAll;
}