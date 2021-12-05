#pragma once
#include <numeric>

template<class T>
class Rational
{
public:
    constexpr Rational() : num_(0), den_(1) { }
    constexpr Rational(T n) : num_(n), den_(1) { }
    constexpr Rational(T n, T m) : num_(n), den_(m) { }

    constexpr Rational & operator+=(T i) 
    { 
        num_ += i * den_; 
        return *this; 
    }

    constexpr Rational & operator-=(T i) 
    { 
        num_ -= i * den_; 
        return *this; 
    }

    constexpr Rational & operator*=(T i)
    {
        T gcd = std::gcd(i, den_);
        num_ *= i / gcd;
        den_ /= gcd;

        return *this;
    }

    constexpr Rational & operator/=(T i)
    {
        if (num_ == 0) 
        {
            return *this;
        }

        T gcd = std::gcd(num_, i);
        num_ *= i / gcd;
        den_ /= gcd;

        return *this;
    }

private:
#define MAKE_BINARY_OP(OP) \
    friend constexpr Rational operator##OP(T s, Rational r) { return r OP= s; } \
    friend constexpr Rational operator##OP(Rational r, T s) { return r OP= s; } \
    friend constexpr Rational operator##OP(Rational r1, const Rational & r2) { return r1 OP= r2; }

    MAKE_BINARY_OP(+)
    MAKE_BINARY_OP(-)
    MAKE_BINARY_OP(*)
    MAKE_BINARY_OP(/)

#undef MAKE_BINARY_OP

    T num_;
    T den_;
};