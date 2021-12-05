#pragma once
#include <type_traits>
#include <concepts>
#include <new>

namespace cont
{
    /**
     * Storage container for a type T.
     * This class bypasses the normal lifetime of the 
     * contained type, meaning that the contained value 
     * is not initialized by the constructor, nor destructed
     * by the destructor. 
     */
    template<class T>
    class Box
    {
    public:
        using value_type = T;

        Box() {}
        ~Box() {}

        template<class ... Args>
        [[maybe_unused]] T & construct(Args && ... args)
        {
            return *std::launder(::new (reinterpret_cast<void *>(&value_)) T(static_cast<Args&&>(args)...));
        }

        template<class F>
            requires requires(F && f) { T(static_cast<F&&>(f)()); }
        [[maybe_unused]] T & constructWith(F && f)
        {
            return *std::launder(::new (reinterpret_cast<void *>(&value_)) T((static_cast<F &&>(f))()));
        }

        void destruct()
        {
            value_.~T();
        }

        /**
         * Get the contained value. Note: calling this on an 
         * uninitialized Box is undefined behavior.
         */
        T & get() & 
        { 
            return value_; 
        }

        T && get() && 
        { 
            return static_cast<T&&>(value_); 
        }
        
        const T & get() const & 
        { 
            return value_; 
        }
        
        const T && get() const && 
        { 
            return static_cast<T&&>(value_); 
        }

    private:
        union {
            T value_;
        };
    };
}