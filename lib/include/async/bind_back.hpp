#pragma once
#include <tuple>
#include <type_traits>
#include <concepts>

namespace async
{
    namespace detail
    {
        // Tag base class to enable checking if a class is a BindBack expression
        struct BindBackBase {};

        template<class T>
        constexpr bool isBindBack = std::is_base_of_v<BindBackBase, std::remove_cvref_t<T>>;

        template<class F, class Arg1>
        struct BindFirstArgument
        {
            F && f_;
            Arg1 && arg1_;

            template<class ... Args>
            auto operator()(Args && ... args)
            {
                return static_cast<F&&>(f_)(static_cast<Arg1&&>(arg1_), static_cast<Args&&>(args)...);
            }
        };

        struct BindBackCompose
        {
            template<class Arg, class Lhs, class Rhs>
            auto operator()(Arg && arg, Lhs && lhs, Rhs && rhs) const
            {
                return 
                    static_cast<Rhs&&>(rhs)
                        ((static_cast<Lhs&&>(lhs))
                            (static_cast<Arg&&>(arg)));
            }
        };
    }

    /**
     * For an invocable F taking N arguments, binds the last N-1 arguments. 
     */
    template<class F, class ... Args>
    class BindBack : public detail::BindBackBase
    {
    public:
        template<class F2>
        BindBack(F2 && f, Args && ... args)
        : f_(static_cast<F2&&>(f))
        , args_(static_cast<Args&&>(args)...)
        {

        }

        template<class Arg>
            requires (!detail::isBindBack<Arg> && std::invocable<const F&, Arg, const Args&...>)
        auto operator()(Arg && arg) const &
        {
            return std::apply(
                detail::BindFirstArgument<const F&, Arg>{f_, static_cast<Arg&&>(arg)}, 
                args_);
        }

        template<class Arg>
            requires (!detail::isBindBack<Arg> && std::invocable<F, Arg, Args...>)
        auto operator()(Arg && arg) &&
        {
            return std::apply(
                detail::BindFirstArgument<F, Arg>{std::move(f_), static_cast<Arg&&>(arg)}, 
                std::move(args_));
        }

        template<class Arg>
            requires (!detail::isBindBack<Arg> && std::invocable<const F&, Arg, const Args&...>)
        friend auto operator|(Arg && arg, const BindBack & self)
        {
            return std::apply(
                detail::BindFirstArgument<const F&, Arg>{self.f_, static_cast<Arg&&>(arg)}, 
                self.args_);
        }

        template<class Arg>
            requires (!detail::isBindBack<Arg> && std::invocable<F, Arg, Args...>)
        friend auto operator|(Arg && arg, BindBack && self)
        {
            return std::apply(
                detail::BindFirstArgument<F, Arg>{std::move(self.f_), static_cast<Arg&&>(arg)}, 
                std::move(self.args_));
        }

        template<class Lhs> 
            requires (detail::isBindBack<Lhs>)
        friend auto operator|(Lhs && lhs, const BindBack & self)
            -> BindBack<detail::BindBackCompose, std::remove_cvref_t<Lhs>, BindBack<F, Args...>>
        {
            return {detail::BindBackCompose{}, static_cast<Lhs&&>(lhs), self};
        }

        template<class Lhs> 
            requires (detail::isBindBack<Lhs>)
        friend auto operator|(Lhs && lhs, BindBack && self)
            -> BindBack<detail::BindBackCompose, std::remove_cvref_t<Lhs>, BindBack<F, Args...>>
        {
            return {detail::BindBackCompose{}, static_cast<Lhs&&>(lhs), std::move(self)};
        }

    private:
        F f_;
        std::tuple<Args...> args_;
    };

    inline constexpr struct bindBack_t
    {
        template<class F, class ... Args>
        auto operator()(F && f, Args && ... args) const 
            -> BindBack<std::remove_cvref_t<F>, std::remove_cvref_t<Args>...>
        {
            return {static_cast<F&&>(f), static_cast<Args&&>(args)...};
        }
    } bindBack{};

    template<class F, class ... Args>
    using BindBackResultType = decltype(bindBack(std::declval<F>(), std::declval<Args>()...));
}