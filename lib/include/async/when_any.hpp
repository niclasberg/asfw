#pragma once
#include "future.hpp"
#include <variant>
#include <type_traits>
#include <tuple>
#include "util.hpp"
#include "tmp/traits.hpp"
#include "tmp/type_list.hpp"
#include "cont/box_union.hpp"

namespace async
{
    namespace detail
    {
        template<class ... Senders>
        using ValueTypeList = tmp::unique_<
            std::conditional_t<
                std::is_void_v<future_value_t<Senders>>,
                tmp::TypeList<>,
                tmp::TypeList<future_value_t<Senders>>
            >...
        >;

        template<class Op, std::size_t I>
        class WhenAnyReceiver
        {
        public:
            WhenAnyReceiver(Op & op) : op_{op} { }

            template<class T>
            void setValue(T &&) && 
            {
                
            }

            template<class E>
            void setError(E &&) && 
            {

            }

            void setDone() &&
            {
                
            }

        private:
            Op & op_;
        };

        template<class R, class ... Senders>
        class WhenAnyOperation
        {
            // Status codes (a code i >= 0 indicates completion with the value type of the ith sender)
            inline static constexpr int RUNNING = -1;
            inline static constexpr int COMPLETED_WITH_ERROR = -2;
            inline static constexpr int COMPLETED_WITH_DONE = -3;

            using Self = WhenAnyOperation<R, Senders...>;
            template<class Op, std::size_t I> friend class WhenAnyReceiver;

            template<std::size_t I>
            using NthReceiverType = WhenAnyReceiver<Self, I>;
            
            using OperationTypeList = decltype(
                []<std::size_t ... Is>(std::index_sequence<Is...>) 
                    -> tmp::TypeList<connect_result_t<Senders, NthReceiverType<Is>>...>
                { return {}; }
                (std::make_index_sequence<sizeof...(Senders)>{})
            );

            using OperationTuple = tmp::apply_<OperationTypeList, std::tuple>;
            using ValueUnion = tmp::apply_<ValueTypeList<Senders...>, cont::BoxUnion>;

        public:
            template<class R2, std::size_t ... Indices, class ... Senders2>
            WhenAnyOperation(R2 && receiver, std::index_sequence<Indices...>, Senders2 && ... senders)
                : receiver_(static_cast<R2&&>(receiver))
                , operationsTuple_(
                    connect(static_cast<Senders2&&>(senders), NthReceiverType<Indices>{*this})...)
            {

            }

            void start() 
            { 
                [this]<std::size_t ... Is>(std::index_sequence<Is...>)
                { 
                    (async::start(std::get<Is>(operationsTuple_)), ...);
                }(std::make_index_sequence<sizeof...(Senders)>{});
            }

        private:
            template<class T>
            void assignValue(T && value)
            {
                value_.construct(cont::union_t<std::remove_cvref_t<T>>, static_cast<T&&>(value));
            }

            [[no_unique_address]] R receiver_;
            [[no_unique_address]] OperationTuple operationsTuple_;
            [[no_unique_address]] ValueUnion value_;
            int completedIndexOrStatus = RUNNING;
        };

        template<class R>
        struct MakeWhenAnyOperation
        {
            template<class ... Senders>
            auto operator()(Senders && ... senders) 
                -> WhenAnyOperation<R, std::remove_cvref_t<Senders>...>
            {
                return { std::move(receiver_), std::make_index_sequence<sizeof...(Senders)>{}, static_cast<Senders&&>(senders)... };
            }

            R receiver_;
        };

        template<AnyFuture ... Senders>
            requires (sizeof...(Senders) > 0) && HasCombinableErrors<Senders...>
        class WhenAnySender
        {
            using Self = WhenAnySender<Senders...>;
        public:
            using value_type = std::conditional_t<
                std::is_same_v<ValueTypeList<Senders...>, tmp::TypeList<>>,
                void,
                tmp::apply_<ValueTypeList<Senders...>, std::variant>
            >;
            using error_type = combined_future_error_t<Senders...>;

            template<class ... Senders2>
                requires (std::is_same_v<std::remove_cvref_t<Senders2>, Senders> && ...)
            constexpr WhenAnySender(Senders2 && ... senders)
            : senders_(static_cast<Senders2&&>(senders)...)
            {

            }
            
            constexpr WhenAnySender(const WhenAnySender &) = default;
            constexpr WhenAnySender(WhenAnySender &&) = default;
            constexpr ~WhenAnySender() = default;

        private:
            template<class S, Receiver<value_type, error_type> R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && self, R && receiver) 
                -> WhenAnyOperation<std::remove_cvref_t<R>, std::remove_cvref_t<Senders>...>
            {
                return std::apply(
                    MakeWhenAnyOperation<std::remove_cvref_t<R>>{ static_cast<R&&>(receiver) },
                    static_cast<S&&>(self).senders_
                );
            }

            std::tuple<Senders...> senders_;
        };
    }

    inline constexpr struct whenAny_t final
    {
        template<class ... Senders>
        constexpr auto operator()(Senders && ... senders) const
            -> detail::WhenAnySender<std::remove_cvref_t<Senders>...>
        {
            return { static_cast<Senders&&>(senders)... };
        }
    } whenAny {};
}