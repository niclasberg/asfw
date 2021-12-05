#pragma once
#include <new>
#include <type_traits>

namespace detail
{
    template<class F, F ptr> struct MemFn {};
    template<class F, F ptr> struct Fn {};
}

template<auto ptr>
inline constexpr detail::MemFn<decltype(ptr), ptr> memFn{};

template<auto ptr>
inline constexpr detail::Fn<decltype(ptr), ptr> fn{};

template<class F>
class Delegate;

template<class R, class ... Args>
class Delegate<R(Args...)>
{
    using FunctionObjectStorage = std::aligned_storage_t<sizeof(void *), alignof(void *)>;
    using FunctionType = R (*)(void *, Args...);

    template<class F> struct FromFunctionObject {};

    struct FunctionStorage
    {
        constexpr FunctionStorage(FunctionType func, void * object)
        : function(func)
        {
            context = object;
        }

        constexpr FunctionStorage(const FunctionStorage &) = default;
        constexpr FunctionStorage(FunctionStorage &&) = default;
        constexpr FunctionStorage & operator=(const FunctionStorage &) = default;
        constexpr FunctionStorage & operator=(FunctionStorage &&) = default;

        FunctionType function;
        void * context;
    };

public:
    constexpr Delegate() : storage_{nullptr, nullptr} { }

    /**
     * Construct from a function pointer (compile-time)
     */
    template<R(*ptr)(Args...)>
    constexpr Delegate(detail::Fn<R(*)(Args...), ptr>)
    : storage_{callFunctionPointer<ptr>, nullptr}
    {

    }

    /**
     * Construct from a member function pointer (compile-time)
     */
    template<class T, R(T::*ptr)(Args...)>
    constexpr Delegate(detail::MemFn<R(T::*)(Args...), ptr>, T & object)
    : storage_{callMemberFunctionPointer<T, ptr>, &object}
    {

    }

    /**
     * Construct from a const member function pointer (compile-time)
     */
    template<class T, R(T::*ptr)(Args...) const>
    constexpr Delegate(detail::MemFn<R(T::*)(Args...) const, ptr>, T & object)
    : storage_{callConstMemberFunctionPointer<T, ptr>, &object}
    {

    }

    /**
     * Construct from function object
     */
    template<class F>
        requires (std::invocable<F, Args...> && !std::same_as<std::decay_t<F>, Delegate>)
    Delegate(F * f)
    : storage_(callFunctionObject<F>, f)
    {

    }

    void reset()
    {
        storage_.function = nullptr;
        storage_.context = nullptr;
    }

    constexpr operator bool() const
    {
        return storage_.function != nullptr;
    }
    
    R operator()(Args ... args) const
    {
        return (storage_.function)(storage_.context, static_cast<Args&&>(args)...);
    }

private:
    constexpr Delegate(FunctionType function, void * object)
    : storage_{function, object}
    {

    }

    template<class F>
    static R callFunctionObject(void * context, Args... args)
    {
        F & f = *static_cast<F *>(context);
        return f(static_cast<Args&&>(args)...);
    }

    template<R(*functionPointer)(Args...)>
    static constexpr R callFunctionPointer(void *, Args... args)
    {
        return functionPointer(static_cast<Args&&>(args)...);
    }

    template<class T, R(T::*ptr)(Args...)>
    static constexpr R callMemberFunctionPointer(void * context, Args... args)
    {
        T * obj = static_cast<T *>(context);
        return (obj->*ptr)(static_cast<Args&&>(args)...);
    }

    template<class T, R(T::*ptr)(Args...) const>
    static constexpr R callConstMemberFunctionPointer(void * context, Args... args)
    {
        T * obj = static_cast<T *>(context);
        return (obj->*ptr)(static_cast<Args&&>(args)...);
    }

    FunctionStorage storage_;
};

// Template deduction guides
template<class T, auto ptr, class R, class ... Args>
Delegate(detail::MemFn<R(T::*)(Args...), ptr> memberFunction, T & object) -> Delegate<R(Args...)>;

template<class T, auto ptr, class R, class ... Args>
Delegate(detail::MemFn<R(T::*)(Args...) const, ptr> memberFunction, const T & object) -> Delegate<R(Args...)>;

template<auto ptr, class R, class ... Args>
Delegate(detail::Fn<R(*)(Args...), ptr>) -> Delegate<R(Args...)>;