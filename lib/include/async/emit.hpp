#pragma once
#include "stream.hpp"
#include "receiver.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class T>
        struct EmitValueStream
        {
            using value_type = T;
            using error_type = void;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver_;
                T value_;

                void start()
                {
                    setNext(receiver_, std::move(value_));
                }

                void next() { setDone(std::move(receiver_)); }
                void stop() { setDone(std::move(receiver_)); }
            };

            template<StreamReceiver<T, void> R>
                requires std::move_constructible<T>
            friend auto tag_invoke(subscribe_t, EmitValueStream<T> && stream, R && receiver)
             -> Operation<std::remove_cvref_t<R>>
            {
                return { static_cast<R&&>(receiver), std::move(stream.value) };
            }

            template<StreamReceiver<T, void> R>
                requires std::copy_constructible<T>
            friend auto tag_invoke(subscribe_t, const EmitValueStream<T> & stream, R && receiver)
             -> Operation<std::remove_cvref_t<R>>
            {
                return { static_cast<R&&>(receiver), stream.value };
            }

            T value;
        };

        template<class E>
        struct EmitErrorStream
        {
            using value_type = void;
            using error_type = E;

            template<class Receiver>
            struct Operation
            {
                Receiver receiver;
                E error_;

                void start()
                {
                    setError(receiver, std::move(error_));
                }

                void next() 
                { 
                    /* This SHOULD never be called, we have called setError
                     * in start, which is a terminal signal */
                }

                void stop() 
                {  
                    /* This SHOULD never be called, we have called setError
                     * in start, which is a terminal signal */
                }
            };

            template<StreamReceiver<void, E> R>
                requires std::move_constructible<T>
            friend auto tag_invoke(subscribe_t, EmitErrorStream<E> && stream, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), std::move(stream.value) };
            }

            template<StreamReceiver<void, E> R>
                requires std::copy_constructible<T>
            friend auto tag_invoke(subscribe_t, const EmitErrorStream<E> & stream, R && receiver)
             -> Operation<std::decay_t<R>>
            {
                return { static_cast<R&&>(receiver), stream.value };
            }

            E value;
        };
    }

    template<class T> 
    constexpr auto emit(T && value) -> detail::EmitValueStream<std::remove_cv_t<T>>
    {
        return { static_cast<T&&>(value) };
    }

    template<class E> 
    constexpr auto emitError(E && error) -> detail::EmitErrorStream<std::remove_cv_t<E>>
    {
        return { static_cast<E&&>(error) };
    }
}