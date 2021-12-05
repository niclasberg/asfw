#pragma once
#include <type_traits>
#include <concepts>
#include <memory>
#include "tmp/parameter_packs.hpp"
#include <new>

namespace cont
{
    template<class T> class union_ {};

    template<class T> 
    constexpr union_<T> union_t{};

    template<class ... Ts>
    class BoxUnion
    {
    public:
        BoxUnion() {}
        ~BoxUnion() {}

        template<class T, class ... Args>
            requires tmp::containsType<T, Ts...>
        [[maybe_unused]] T & construct(union_<T>, Args && ... args)
        {
            T * ptr = ::new (static_cast<void*>(&storage_)) T(static_cast<Args&&>(args)...);
            return *std::launder(ptr);
        }

        template<std::invocable F, class R = std::invoke_result_t<F>>
            requires tmp::containsType<R, Ts...>
        [[maybe_unused]] R & constructWith(F && f)
        {
            return *std::launder(::new (reinterpret_cast<void *>(&storage_)) R((static_cast<F &&>(f))()));
        }

        template<class T> 
            requires tmp::containsType<T, Ts...>
        void destruct(union_<T>)
        {
            static_cast<T*>(static_cast<void*>(&storage_))->~T();
        }

        template<class T> 
            requires tmp::containsType<T, Ts...>
        T & get(union_<T>)
        {
            return *std::launder(reinterpret_cast<T *>(&storage_));
        }

    private:
        std::aligned_union_t<0, Ts...> storage_;
    };
}