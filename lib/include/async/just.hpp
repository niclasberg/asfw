#pragma once
#include "async/receiver.hpp"
#include <type_traits>

namespace async
{
    namespace detail
    {
        template<class ... Ts>
        struct JustValueSender
        {
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using value_types = Variant<Tuple<Ts...>>;
            
            template<template<typename...> typename Variant> 
            using error_types = Variant<>;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver;
                std::tuple<Ts...> values_;

                void start()
                {
                    std::apply([this](Ts ... values) {
                        setValue(std::move(receiver), values...);
                    }, std::move(values_));
                }
            };

            template<class R>
                requires ManyReceiverOf<R, Ts...>
            Operation<std::decay_t<R>> connect(R && receiver) &&
            {
                return {static_cast<R&&>(receiver), std::move(values)};
            }

            template<class R>
                requires ManyReceiverOf<R, Ts...>
            Operation<std::decay_t<R>> connect(R && receiver) const &
            {
                return {static_cast<R&&>(receiver), values};
            }

            std::tuple<Ts...> values;
        };

        template<class ... Ts>
        struct JustNextSender
        {
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using next_types = Variant<Tuple<Ts...>>;
            template<template<typename...> typename Tuple> 
            using error_types = Tuple<>;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver;
                std::tuple<Ts...> ts;

                void start()
                {
                    std::apply([this](Ts ... values) {
                        setNext(receiver, static_cast<Ts&&>(values)...);
                    }, ts);
                    setDone(std::move(receiver));
                }
            };

            template<class R>
                requires ManyReceiverOf<R, Ts...>
            Operation<std::decay_t<R>> connect(R && receiver) &&
            {
                return {static_cast<R&&>(receiver), std::move(values)};
            }

            std::tuple<Ts...> values;
        };

        template<class E>
        struct JustErrorSender
        {
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using value_types = Variant<Tuple<>>;
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using next_types = Variant<Tuple<>>;
            template<template<typename...> typename Tuple> 
            using error_types = Tuple<E>;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver_;
                E error_;

                void start()
                {
                    setError(std::move(receiver_), std::move(error_));
                }
            };

            template<class R>
            Operation<std::remove_cvref_t<R>> connect(R && receiver) &&
            {
                return {static_cast<R&&>(receiver), std::move(error_)};
            }

            E error_;
        };

        struct JustDoneSender
        {
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using value_types = Variant<Tuple<>>;
            template<
                template<typename...> typename Variant,
                template<typename...> typename Tuple>
            using next_types = Variant<Tuple<>>;
            template<template<typename...> typename Tuple> 
            using error_types = Tuple<>;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver_;

                void start()
                {
                    setDone(std::move(receiver_));
                }
            };

            template<class R>
            Operation<std::remove_cvref_t<R>> connect(R && receiver) &&
            {
                return {static_cast<R&&>(receiver)};
            }
        };
    }

    /**
     * Creates a sender that, once started, calls setValue on the reciever 
     * with the provided values 
     * 
     * @tparam Ts Value types
     * @param ts Values to emit
     * @return detail::JustValueSender<std::remove_cvref_t<Ts>...> 
     */
    template<class ... Ts>
    constexpr auto justValue(Ts && ... ts) -> detail::JustValueSender<std::remove_cvref_t<Ts>...>
    {
        return {std::forward_as_tuple(ts...)};
    }

    /**
     * Creates a ManySender that, once start, calls setNext on the connected receiver
     * with the provided values, and then setDone.
     * 
     * @tparam Ts 
     * @param ts 
     * @return detail::JustNextSender<std::remove_cvref_t<Ts>...> 
     */
    template<class ... Ts>
    constexpr auto justNext(Ts && ... ts) -> detail::JustNextSender<std::remove_cvref_t<Ts>...>
    {
        return {std::forward_as_tuple(ts...)};
    }

    template<class E>
    constexpr auto justError(E && error) -> detail::JustErrorSender<std::remove_cvref_t<E>>
    {
        return {static_cast<E&&>(error)};
    }

    constexpr auto justDone() -> detail::JustDoneSender
    {
        return {};
    }
}