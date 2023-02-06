#pragma once
#include <type_traits>
#include <functional>
#include "future.hpp"
#include "stream.hpp"
#include "receiver.hpp"
#include "bind_back.hpp"
#include "tmp/traits.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class R, class FValue, class FError>
        class MapReceiver
        {
        public:
            template<class R2, class FValue2, class FError2>
            MapReceiver(R2 && receiver, FValue2 && fValue, FError2 && fError)
                : receiver_(static_cast<R2&&>(receiver))
                , fValue_(static_cast<FValue2&&>(fValue))
                , fError_(static_cast<FError2&&>(fError))
            {

            }

            template<class T>
            void setValue(T && value) &&
            {
                using Result = tmp::CallableResultType<FValue, T>;
                if constexpr (std::is_void_v<Result>)
                {
                    std::move(fValue_)(static_cast<T &&>(value));
                    async::setValue(std::move(receiver_), tmp::Void{});
                }
                else
                {   
                    async::setValue(
                        std::move(receiver_), 
                        std::move(fValue_)(static_cast<T &&>(value)));
                }
            }

            void setValue(tmp::Void &&) &&
            {
                using Result = tmp::CallableResultType<FValue, void>;
                if constexpr (std::is_void_v<Result>)
                {
                    std::move(fValue_)();
                    async::setValue(std::move(receiver_), tmp::Void{});
                }
                else
                {   
                    async::setValue(std::move(receiver_), std::move(fValue_)());
                }
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(std::move(receiver_), std::move(fError_)(static_cast<E &&>(e)));
            }

            void setDone() &&
            {
                async::setDone(std::move(receiver_));
            }

        private:
            template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, const MapReceiver & self, Args &&... args) 
                -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
            {
                return cpo(self.receiver_, static_cast<Args&&>(args)...);
            }

            [[no_unique_address]] R receiver_;
            [[no_unique_address]] FValue fValue_;
            [[no_unique_address]] FError fError_;
        };

        template<class R, class FValue, class FError>
        class MapStreamReceiver
        {
        public:
            template<class R2, class FValue2, class FError2>
            MapStreamReceiver(R2 && receiver, FValue2 && fValue, FError2 && fError)
                : receiver_(static_cast<R2&&>(receiver))
                , fValue_(static_cast<FValue2&&>(fValue))
                , fError_(static_cast<FError2&&>(fError))
            {

            }

            template<class T>
            void setNext(T && value) &
            {
                using Result = tmp::CallableResultType<FValue, T>;
                if constexpr (std::is_void_v<Result>)
                {
                    fValue_(static_cast<T &&>(value));
                    async::setNext(receiver_, tmp::Void{});
                }
                else
                {   
                    async::setNext(receiver_, fValue_(static_cast<T &&>(value)));
                }
            }

            void setNext(tmp::Void &&) &
            {
                using Result = tmp::CallableResultType<FValue, void>;
                if constexpr (std::is_void_v<Result>)
                {
                    fValue_();
                    async::setValue(receiver_, tmp::Void{});
                }
                else
                {   
                    async::setValue(receiver_, fValue_());
                }
            }

            template<class E>
            void setError(E && e) &&
            {
                async::setError(
                    std::move(receiver_), 
                    std::move(fError_)(static_cast<E &&>(e)));
            }

            void setDone() &&
            {
                async::setDone(std::move(receiver_));
            }

        private:
            template<class Cpo, class ... Args>
            friend auto tag_invoke(Cpo cpo, const MapStreamReceiver & self, Args &&... args) 
                -> decltype(cpo(std::declval<const R&>(), static_cast<Args&&>(args)...))
            {
                return cpo(self.receiver_, static_cast<Args&&>(args)...);
            }

            [[no_unique_address]] R receiver_;
            [[no_unique_address]] FValue fValue_;
            [[no_unique_address]] FError fError_;
        };

        template<AnyFuture ParentSender, class FValue, class FError>
        class MapFuture
        {
            using Self = MapFuture<ParentSender, FValue, FError>;
        public:
            using value_type = std::remove_cvref_t<tmp::CallableResultType<FValue, future_value_t<ParentSender>>>;
            using error_type = std::remove_cvref_t<tmp::CallableResultType<FError, future_error_t<ParentSender>>>;

            template<class ParentSender2, class FValue2, class FError2>
            MapFuture(ParentSender2 && parentSender, FValue2 && fValue, FError2 && fError)
            : parentSender_(static_cast<ParentSender &&>(parentSender))
            , fValue_(static_cast<FValue2 &&>(fValue))
            , fError_(static_cast<FError2 &&>(fError))
            {

            }
            
        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && sender, R && receiver)
            {
                return async::connect(
                    static_cast<S&&>(sender).parentSender_, 
                    MapReceiver<std::remove_cvref_t<R>, FValue, FError>{
                        static_cast<R &&>(receiver), 
                        static_cast<S&&>(sender).fValue_,
                        static_cast<S&&>(sender).fError_});
            }

            [[no_unique_address]] ParentSender parentSender_;
            [[no_unique_address]] FValue fValue_;
            [[no_unique_address]] FError fError_;
        };

        template<AnyStream ParentStream, class FValue, class FError>
        class MapStream
        {
            using Self = MapStream<ParentStream, FValue, FError>;
        public:
            using value_type = std::remove_cvref_t<tmp::CallableResultType<FValue, stream_value_t<ParentStream>>>;
            using error_type = std::remove_cvref_t<tmp::CallableResultType<FError, stream_error_t<ParentStream>>>;

            template<class ParentStream2, class FValue2, class FError2>
            MapStream(ParentStream2 && parentStream, FValue2 && fValue, FError2 && fError)
            : parentStream_(static_cast<ParentStream &&>(parentStream))
            , fValue_(static_cast<FValue2 &&>(fValue))
            , fError_(static_cast<FError2 &&>(fError))
            {

            }
            
        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(subscribe_t, S && sender, R && receiver)
            {
                return async::subscribe(
                    static_cast<S&&>(sender).parentSender_, 
                    MapStreamReceiver<std::remove_cvref_t<R>, FValue, FError>{
                        static_cast<R &&>(receiver), 
                        static_cast<S&&>(sender).fValue_,
                        static_cast<S&&>(sender).fError_});
            }

            [[no_unique_address]] ParentStream parentStream_;
            [[no_unique_address]] FValue fValue_;
            [[no_unique_address]] FError fError_;
        };        
    }

    /**
     * @brief Map a future's value to a different type. The function `f`
     * will only be invoked if the future completes with a value (not
     * if it completes with an error or is cancelled).
     * 
     */
    inline constexpr struct map_t 
    {
        template<AnyFuture S, class F>
            requires tmp::Callable<F, future_value_t<S>>
        auto operator()(S && parent, F && f) const
            -> detail::MapFuture<std::remove_cvref_t<S>, std::remove_cvref_t<F>, tmp::Identity>
        {
            return {static_cast<S &&>(parent), static_cast<F &&>(f), tmp::Identity{}};
        }

        template<AnyStream S, class F>
            requires tmp::Callable<F, stream_value_t<S>>
        auto operator()(S && parent, F && f) const
            -> detail::MapStream<std::remove_cvref_t<S>, std::remove_cvref_t<F>, tmp::Identity>
        {
            return {static_cast<S &&>(parent), static_cast<F &&>(f), tmp::Identity{}};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<map_t, F>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } map {};

    inline constexpr struct mapError_t 
    {
        template<AnyFuture S, class F>
            requires tmp::Callable<F, future_error_t<S>>
        auto operator()(S && parent, F && f) const
            -> detail::MapFuture<std::remove_cvref_t<S>, tmp::Identity, std::remove_cvref_t<F>>
        {
            return {static_cast<S &&>(parent), tmp::Identity{}, static_cast<F &&>(f)};
        }

        template<AnyStream S, class F>
            requires tmp::Callable<F, stream_error_t<S>>
        auto operator()(S && parent, F && f) const
            -> detail::MapFuture<std::remove_cvref_t<S>, tmp::Identity, std::remove_cvref_t<F>>
        {
            return {static_cast<S &&>(parent), tmp::Identity{}, static_cast<F &&>(f)};
        }

        template<class F>
        auto operator()(F && f) const -> BindBackResultType<mapError_t, F>
        {
            return bindBack(*this, static_cast<F &&>(f));
        }
    } mapError {};
}