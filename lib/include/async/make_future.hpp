#pragma once
#include "future.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class F, class T, class E>
        class SourceFuture
        {
            using Self = SourceFuture<F, T, E>;
        public:
            using value_type = T;
            using error_type = E;

            template<class F2>
            SourceFuture(F2 && factory) : factory_(static_cast<F2&&>(factory)) { }

            SourceFuture(SourceFuture &&) = default;
            SourceFuture(const SourceFuture &) = default;
            ~SourceFuture() = default;

        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(connect_t, S && sender, R && receiver)
                -> decltype(std::declval<F&&>()(std::declval<R&&>()))
            {
                return (static_cast<S&&>(sender).factory_)(static_cast<R&&>(receiver));
            }

            F factory_;
        };
    }

    template<class T, class E>
    struct makeFuture_t final
    {
        template<class F>
        auto operator()(F && factory) const
            -> detail::SourceFuture<std::remove_cvref_t<F>, T, E>
        {
            return { static_cast<F&&>(factory) };
        }
    };
    
    template<class T, class E>
    inline constexpr makeFuture_t<T, E> makeFuture{};
}