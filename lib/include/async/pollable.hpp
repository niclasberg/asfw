#pragma once
#include <memory>
#include "tmp/traits.hpp"
#include "outcome.hpp"

namespace async
{
    enum class PollResult
    {
        PENDING,
        SUCCESS,
        ERROR
    };

    /**
     * @brief A pollable is similar to std::future in that it represents
     * a value (or an error) that will be available at a later point. Its main use
     * case is to construct lightweight state machines for resource constrained
     * devices, allowing for a simple form of concurrency. 
     * 
     * In order to evaluate the pollable, the poll() method should be called. When the operation
     * has yet to complete, a PollResult::PENDING will be returned. After completing,
     * polling will return either a PollResult::SUCCESS or a PollResult::ERROR. Note 
     * that the poll() method should be idempotent.
     * 
     * After completing with a SUCCESS status, the value can be extracted by calling 
     * the value() method. Calling value() on a non-successful pollable is 
     * undefined behavior. Similarily, if the pollable completed with an ERROR status,
     * the error() method can be used to obtain the error.
     * 
     * End users will usually not have to deal directly with Pollables, but should
     * use the combinators (map, andThen, sequence and so on), and then use the waitFor 
     * function to busy-wait for the result.
     * 
     * @tparam F 
     */
    template<class F>
    concept AnyPollable = requires (std::remove_cvref_t<F> & f) {
        typename F::value_type;
        typename F::error_type;
        { f.poll() } -> std::same_as<PollResult>;
        { f.value() } -> std::same_as<typename F::value_type>;
    };

    /**
     * @brief Get the value type for a pollable
     * 
     * @tparam F Pollable
     */
    template<AnyPollable F>
    using pollable_value_t = typename std::remove_cvref_t<F>::value_type;

    /**
     * @brief Get the error type for a pollable
     * 
     * @tparam F Pollable
     */
    template<AnyPollable F>
    using pollable_error_t = typename std::remove_cvref_t<F>::error_type;

    /**
     * @brief A pollable with a specified value and error type
     * 
     * @tparam F Pollable
     * @tparam T Value type
     * @tparam E Error type (or void if the pollable cannot fail)
     */
    template<class F, class T, class E>
    concept Pollable = 
        AnyPollable<F> &&
        std::same_as<pollable_value_t<F>, T> &&
        std::same_as<pollable_error_t<F>, E> &&
        (std::same_as<E, void> || requires(std::remove_cvref_t<F> & f) {
            { f.error() } -> std::convertible_to<E>;
        });

    inline constexpr struct waitFor_t final
    {
        template<AnyPollable F>
        constexpr auto operator()(F && fut) const 
            -> Outcome<pollable_value_t<F>, pollable_error_t<F>>
        {
            using T = pollable_value_t<F>; 
            using E = pollable_error_t<F>;

            for (;;) 
            {
                auto res = fut.poll();
                if (res == PollResult::SUCCESS)
                {
                    if constexpr (std::is_void_v<T>)
                    {
                        fut.value(); // Evaluate value side-effects
                        return makeSuccess<E>();
                    }
                    else
                        return makeSuccess<E>(fut.value());
                }

                if constexpr (not std::is_void_v<E>)
                {
                    if (res == PollResult::ERROR)
                        return makeError<T>(fut.error());
                }
            }
        }
    } waitFor {};

    struct EmptyPollable
    {
        using value_type = void;
        using error_type = void;

        constexpr PollResult poll() const
        { 
            return PollResult::SUCCESS;
        }

        constexpr void value() const { }
    };

    template<class T>
    class ValuePollable
    {
    public:
        using value_type = T;
        using error_type = void;

        template<class T2>
            requires std::same_as<std::remove_cvref_t<T2>, T>
        constexpr ValuePollable(T2 && value) : value_(static_cast<T2&&>(value)) { }

        constexpr PollResult poll() const
        { 
            return PollResult::SUCCESS;
        }

        constexpr T value() const { return value_; }

    private:
        T value_;
    };

    template<class E>
    class ErrorPollable
    {
    public:
        using value_type = void;
        using error_type = E;

        template<class E2>
            requires std::same_as<std::remove_cvref_t<E2>, E>
        constexpr ErrorPollable(E2 && error) : error_(static_cast<E2&&>(error)) { }

        constexpr ErrorPollable(const ErrorPollable &) = default;
        constexpr ErrorPollable(ErrorPollable &&) = default;
        constexpr ~ErrorPollable() = default;

        constexpr PollResult poll() const
        { 
            return PollResult::ERROR;
        }

        constexpr void value() const { }
        constexpr E error() const { return error_; }

    private:
        E error_;
    };

    inline constexpr struct makeValue_t final
    {
        template<class T>
        constexpr auto operator()(T && value) const -> ValuePollable<std::remove_cvref_t<T>>
        {
            return { static_cast<T&&>(value) };
        }
    } makeValue {};

    template<AnyPollable S, class F>
        requires tmp::Callable<F, typename S::value_type>
    class MapPollable
    { 
        using prev_value_type = typename S::value_type;
    public:
        using value_type = tmp::CallableResultType<F, prev_value_type>;
        using error_type = typename S::error_type;

        template<class S2, class F2>
            requires std::same_as<std::remove_cvref_t<S2>, S> && 
                std::same_as<std::remove_cvref_t<F2>, F>
        constexpr MapPollable(S2 && prev, F2 && f)
        : prev_(static_cast<S2&&>(prev)), f_(static_cast<F2&&>(f))
        {

        }

        constexpr MapPollable(const MapPollable &) = default;
        constexpr MapPollable(MapPollable &&) = default;

        constexpr PollResult poll()
        {
            return prev_.poll();
        }

        constexpr value_type value() const 
        { 
            if constexpr (std::is_void_v<prev_value_type>)
            {
                prev_.value();
                if constexpr (std::is_void_v<value_type>)
                    f_();
                else
                    return f_();
            }
            else 
            {
                if constexpr (std::is_void_v<value_type>)
                    f_(prev_.value());
                else
                    return f_(prev_.value());
            }
        }

        constexpr error_type error() const 
            requires(not std::is_void_v<error_type>)
        { 
            return prev_.error();
        }

    private:
        [[no_unique_address]] S prev_;
        [[no_unique_address]] F f_;
    };

    inline constexpr struct map_t final
    {
        template<AnyPollable S, class F>
        constexpr auto operator()(S && prev, F && f) const 
            -> MapPollable<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return { static_cast<S&&>(prev), static_cast<F&&>(f) };
        }
    } map {};

    template<AnyPollable P, class F>
        requires tmp::CallableR<F, pollable_error_t<P>, pollable_value_t<P>> 
            //&& not std::same_as<pollable_error_t<P>, void>
    class HandleErrorPollable
    {
    public:
        using value_type = pollable_value_t<P>;
        using error_type = void;

        constexpr void poll() { return prev_.poll(); }

        constexpr value_type value() 
        { 
            if (prev_.poll() == PollResult::ERROR)
                return errorHandler_(prev_.error());
            else 
                return prev_.value();
        }

    private:
        [[no_unique_address]] P prev_;
        [[no_unique_address]] F errorHandler_;
    };

    template<AnyPollable PFirst, class F, class PSecond = tmp::CallableResultType<F, pollable_value_t<PFirst>>>
        requires AnyPollable<PSecond>
    class FlatMapFut
    { 
    public:
        using value_type = typename PSecond::value_type;
        using error_type = typename PSecond::error_type;

        template<std::convertible_to<PFirst> PFirst2, std::convertible_to<F> F2>
        constexpr FlatMapFut(PFirst2 && first, F2 && f)
        : first_(static_cast<PFirst2&&>(first)), f_(static_cast<F2&&>(f))
        {

        }

        constexpr ~FlatMapFut()
        {
            if (firstCompleted_)
                second_.~PSecond();
            else
                first_.~PFirst();
        }

        constexpr PollResult poll()
        {
            if (!firstCompleted_)
            {
                PollResult res = first_.poll();
                if (res == PollResult::PENDING || res == PollResult::ERROR)
                    return res;
                else // SUCCESS
                {
                    if constexpr (std::is_void_v<pollable_value_t<PFirst>>)
                    {
                        // We still evaluate the value here, there might be side effects
                        // that are evaluated
                        first_.value(); 
                        first_.~PFirst();
                        std::construct_at(std::addressof(second_), std::move(f_)());
                    }
                    else
                    {
                        auto value = first_.value();
                        first_.~PFirst();
                        std::construct_at(std::addressof(second_), std::move(f_)(std::move(value)));
                    }
                        
                    firstCompleted_ = true;
                    return second_.poll();
                }
            }   
            else
            {
                return second_.poll();
            }
        }

        constexpr auto value() const -> decltype(std::declval<PSecond>().value())
        {
            return second_.value();
        }

        constexpr error_type error() const 
            requires(not std::is_void_v<error_type>)
        {
            return second_.error();
        }

    private:
        bool firstCompleted_ = false;
        [[no_unique_address]] F f_;
        union {
            [[no_unique_address]] PFirst first_;
            [[no_unique_address]] PSecond second_;
        };
    };

    inline constexpr struct flatMap_t final
    {
        template<AnyPollable S, class F>
        constexpr auto operator()(S && prev, F && f) const 
            -> FlatMapFut<std::remove_cvref_t<S>, std::remove_cvref_t<F>>
        {
            return { static_cast<S&&>(prev), static_cast<F&&>(f) };
        }
    } flatMap {};
}