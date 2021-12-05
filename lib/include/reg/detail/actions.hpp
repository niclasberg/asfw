#pragma once

namespace reg::detail
{
    

    template<class T, class _Mask>
    class BitIsSetAction : _Mask
    {
    public:
        template<template<typename...> class Tuple>
        using ValueTypes = Tuple<bool>;

        template<class ... Args>
        constexpr BitIsSetAction(Args && ... args) 
        : _Mask(std::forward<Args>(args)...), output(0)
        {

        }

        constexpr Value operator()(Value value) const
        {
            output = value | _Mask::get();
            return value;
        }
        
        bool output;
    };

    
}