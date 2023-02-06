#pragma once
#include "future.hpp"
#include "async/receiver.hpp"
#include <type_traits>

namespace async
{
    namespace detail
    {
        template<class T>
        struct JustValueSender
        {
            using value_type = T;
            using error_type = void;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver;
                T value_;

                void start()
                {
                    setValue(std::move(receiver), std::move(value_));
                }
            };

            template<Receiver<T, void> R>
                requires std::move_constructible<T>
            friend auto tag_invoke(connect_t, JustValueSender<T> && sender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), std::move(sender.value) };
            }

            template<Receiver<T, void> R>
                requires std::copy_constructible<T>
            friend auto tag_invoke(connect_t, const JustValueSender<T> & sender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), sender.value };
            }

            T value;
        };

        class EmptyValueSender
        {
        public:
            using value_type = void;
            using error_type = void;

            template<class R>
            struct Operation
            {
                R receiver;

                void start()
                {
                    setValue(std::move(receiver), tmp::Void{});
                }
            };

        private:
            template<Receiver<void, void> R>
            friend auto tag_invoke(connect_t, EmptyValueSender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver) };
            }
        };

        template<class E>
        struct JustErrorSender
        {
            using value_type = void;
            using error_type = E;

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

            template<Receiver<void, E> R>
                requires std::move_constructible<E>
            friend auto tag_invoke(connect_t, JustErrorSender<E> && sender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), std::move(sender.error) };
            }

            template<Receiver<void, E> R>
                requires std::copy_constructible<E>
            friend auto tag_invoke(connect_t, const JustErrorSender<E> & sender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), sender.error };
            }

            E error;
        };

        struct JustDoneSender
        {
            using value_type = void;
            using error_type = void;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver_;

                void start()
                {
                    setDone(std::move(receiver_));
                }
            };

            template<Receiver<void, void> R>
            friend auto tag_invoke(connect_t, JustDoneSender, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver) };
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
    template<class T>
    constexpr auto just(T && t) -> detail::JustValueSender<std::remove_cvref_t<T>>
    {
        return { static_cast<T&&>(t) };
    }

    /**
     * @brief Create a sender that, once started, emits a void value
     * 
     * @return detail::EmptyValueSender 
     */
    constexpr detail::EmptyValueSender just()
    {
        return {};
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