#pragma once
#include <numeric>

template <class T>
class Rational
{
public:
    constexpr Rational() : num(0), den(1) {}
    constexpr explicit Rational(T n) : num(n), den(1) {}
    
    constexpr Rational(T n, T m) : num(n), den(m)
    {
        
    }

    constexpr Rational(const Rational &) = default;

    // Scalar-rational comparion operators
    constexpr bool operator==(T s) const
    {
        return num == (s * den);
    }

    constexpr bool operator<(T s) const
    {
        // FIXME this only works for positive numbers
        return (num / den) < s;
    }

    constexpr bool operator<=(T s) const 
    { 
        return (operator<(s) || operator==(s));
    }

    constexpr bool operator>(T s) const 
    { 
        return !(operator<(s) || operator==(s));
    }

    constexpr bool operator>=(T s) const 
    { 
        return !(operator<(s));
    }

    const T num;
    const T den;

private:
    friend constexpr Rational operator+(Rational r, T s) 
    { 
        return Rational<T>{
            r.num + s * r.den, 
            r.den};
    }

    friend constexpr Rational operator+(T s, Rational r) 
    { 
        return r + s; 
    }

    friend constexpr Rational operator-(Rational r, T s) 
    { 
        return Rational<T>{
            r.num - s * r.den,
            r.den}; 
    }

    friend constexpr Rational operator-(T s, Rational r) 
    { 
        return Rational{
            s * r.den - r.num,
            r.den}; 
    }

    friend constexpr Rational operator*(Rational r, T s) 
    { 
        const T gcd = std::gcd(s, r.den);
        return Rational{
            r.num * (s / gcd), 
            r.den / gcd};
    }

    friend constexpr Rational operator*(T s, Rational r) 
    { 
        return r * s;
    }

    friend constexpr Rational operator/(Rational r, T s) 
    { 
        if (r.num == 0)
        {
            return r;
        }

        const T gcd = std::gcd(r.num, s);
        return Rational{
            r.num / gcd,
            r.den * (s / gcd)};
    }

    friend constexpr Rational operator*(Rational lhs, const Rational &rhs)
    {
        const T gcd1 = std::gcd(lhs.num, rhs.den);
        const T gcd2 = std::gcd(rhs.num, lhs.den);

        return Rational{
            (lhs.num / gcd1) * (rhs.num / gcd2),
            (lhs.den / gcd2) * (rhs.den / gcd1)};
    }

    friend constexpr Rational operator/(Rational lhs, const Rational &rhs)
    {
        return Rational{
           lhs.num *  rhs.den,
           lhs.den * rhs.num};
    }
};

template<class T>
constexpr T round(const Rational<T> & r)
{
    return ((T(10) * r.num + T(5)) / r.den) / T(10);
}