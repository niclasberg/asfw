#pragma once
#include "inplace_stop_token.hpp"

namespace async
{
    namespace detail
    {
        template<class R>
        struct UseStopSourceReceiver
        {
            R innerReceiver_; 
            InplaceStopSource & stopSource_; 

            template<class ... Values>
                requires ReceiverOf<R, Values...>
            void setValue(Values && ... values) &&
            {
                async::setValue(std::move(innerReceiver_), static_cast<Values&&>(values)...);
            }

            template<class ... Values>
                requires ManyReceiverOf<R, Values...>
            void setNext(Values && ... values)
            {
                async::setNext(innerReceiver_, static_cast<Values&&>(values)...);
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(innerReceiver_), static_cast<E&&>(e));
            }

            void setDone() &&
            {
                async::setDone(std::move(innerReceiver_));
            }

            friend auto tag_invoke(getStopToken_t, const UseStopSourceReceiver & self) -> InplaceStopToken
            {
                return self.stopSource_.getToken();
            }

            template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, const UseStopSourceReceiver & self, Args &&... args) 
                -> decltype(cpo(std::declval<R>(), static_cast<Args&&>(args)...))
            {
                return cpo(self.innerReceiver_, static_cast<Args&&>(args)...);
            }
        };

        template<class ParentSender, class R>
        class UseStopSourceOperation
        {
            using InnerOperation = ConnectResultType<ParentSender, UseStopSourceReceiver<R>>;

        public:
            template<class R2, class F2>
            UseStopSourceOperation(R2 && receiver, F2 && senderFactory)
            : innerOperation_(
                connect(
                    static_cast<F2&&>(senderFactory)(stopSource_),
                    UseStopSourceReceiver<R>{static_cast<R2&&>(receiver), stopSource_}
                ))
            {

            }

            UseStopSourceOperation(const UseStopSourceOperation &) = delete;
            UseStopSourceOperation(UseStopSourceOperation &&) = delete;
            UseStopSourceOperation & operator=(const UseStopSourceOperation &) = delete;
            UseStopSourceOperation & operator=(UseStopSourceOperation &&) = delete;

            void start()
            {
                innerOperation_.start();
            }
        
        private:
            InplaceStopSource stopSource_;
            InnerOperation innerOperation_;
        };

        template<class SenderFactory>
        struct UseStopSourceSender
        {
        public:
            using InnerSender = std::invoke_result_t<SenderFactory, InplaceStopSource &>;
            
            template<class R>
            auto connect(R && receiver)
                -> UseStopSourceOperation<InnerSender, std::remove_cvref_t<R>>
            {
                return {static_cast<R&&>(receiver), std::move(senderFactory_)};
            }

            SenderFactory senderFactory_;
        };
    }

    template<class R>
    auto addStopSourceToReceiver(R && receiver, InplaceStopSource & stopSource) -> detail::UseStopSourceReceiver<std::remove_cvref_t<R>>
    {
        return {static_cast<R&&>(receiver), stopSource};
    }

    template<class SenderFactory>
        requires requires(SenderFactory && sf, InplaceStopSource & stopToken) {
            sf(stopToken);
        }
    auto useStopSource(SenderFactory && senderFactory)
        -> detail::UseStopSourceSender<std::remove_cvref_t<SenderFactory>>
    {
        return {static_cast<SenderFactory&&>(senderFactory)};
    }
}