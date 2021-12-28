#pragma once
#include "sender.hpp"

namespace async
{
    namespace detail
    {
        template<class F, class E, class ... Args>
        class SourceSender
        {
        public:
            template<
                template<typename ...> typename Variant, 
                template<typename ...> typename Tuple>
            using value_types = Variant<Tuple<Args...>>;

            template<template<typename ...> typename Variant> 
            using signal_types = Variant<>;

            template<template<typename ...> typename Variant> 
            using error_types = Variant<E>;

            template<class F2>
            SourceSender(F2 && factory) : factory_(static_cast<F2&&>(factory)) { }

            template<class R>
            auto connect(R && receiver) &&
                -> decltype(std::declval<F&&>()(std::declval<R&&>()))
            {
                return std::move(factory_)(static_cast<R&&>(receiver));
            }

            template<class R>
            auto connect(R && receiver) const &
                -> decltype(std::declval<F>()(std::declval<R&&>()))
            {
                return factory_(static_cast<R&&>(receiver));
            }

        private:
            F factory_;
        };

        template<class F, class E, class ... Args>
        class SignalSourceSender
        {
        public:
            template<template<typename ...> typename Variant, template<typename ...> typename Tuple>
            using value_types = Variant<Tuple<>>;

            template<template<typename ...> typename Variant>
            using signal_types = Variant<Args...>;

            template<template<typename ...> typename Tuple> 
            using error_types = Tuple<E>;

            template<class F2>
            SignalSourceSender(F2 && factory) : factory_(static_cast<F2&&>(factory)) { }

            template<class R>
            auto connect(R && receiver) &&
            {
                return std::move(factory_)(static_cast<R&&>(receiver));
            }

        private:
            F factory_;
        };
    }

    template<class E, class ... ValueTypes>
    struct makeSourceSender_t 
    {
        template<class F>
        auto operator()(F && factory) const
            -> detail::SourceSender<std::remove_cvref_t<F>, E, ValueTypes...>
        {
            return {static_cast<F&&>(factory)};
        }
    };
    
    template<class E, class ... ValueTypes>
    inline constexpr makeSourceSender_t<E, ValueTypes...> makeSourceSender{};

    template<class E, class ... SignalTypes>
    struct makeSignalSourceSender_t 
    {
        template<class F>
        auto operator()(F && factory) const
            -> detail::SignalSourceSender<std::remove_cvref_t<F>, E, SignalTypes...>
        {
            return {static_cast<F&&>(factory)};
        }
    };
    
    /**
     * Constructs a ManySender with \p E as error_type
     * and \p ValueTypes as value_type.
     */
    template<class E, class ... ValueTypes>
    inline constexpr makeSignalSourceSender_t<E, ValueTypes...> makeSignalSourceSender{};
}