#pragma once
#include <type_traits>

/**
 * Contains a value or an error
 * @tparam T value type
 * @tparam E error type
 */
template<class T, class E>
class Result
{
public:
    Result() = delete;
    explicit constexpr Result(const T & value) : result_(value), hasValue_(true) { }
    explicit constexpr Result(T && value) : result_(static_cast<T&&>(value)), hasValue_(true) { }
    explicit constexpr Result(const E & error) : error_(error), hasValue_(false) { }
    explicit constexpr Result(E && error) : error_(static_cast<E&&>(error)), hasValue_(false) { }
    ~Result() = default;

    constexpr bool hasValue() const
    {
        return hasValue_;
    }

    constexpr operator bool() const
    {
        return hasValue_;
    }

    const T & value() const
    {
        return result_;
    }

    template<class U>
    T valueOr(U && defaultValue) &&
    {
        return hasValue ? std::move(result_) : static_cast<U&&>(defaultValue);
    }

private:
    union 
    {
        T result_;
        E error_;
    };
    bool hasValue_;
};