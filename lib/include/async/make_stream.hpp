#pragma once
#include "stream.hpp"
#include "tmp/tag_invoke.hpp"

namespace async
{
    namespace detail
    {
        template<class F, class T, class E>
        class SourceStream
        {
            using Self = SourceStream<F, T, E>;
        public:
            using value_type = T;
            using error_type = E;

            template<class F2>
            SourceStream(F2 && factory) : factory_(static_cast<F2&&>(factory)) { }

        private:
            template<class S, class R>
                requires std::same_as<std::remove_cvref_t<S>, Self>
            friend auto tag_invoke(subscribe_t, S && sender, R && receiver)
            {
                return (static_cast<S&&>(sender).factory_)(static_cast<R&&>(receiver));
            }

            F factory_;
        };
    }

    template<class T, class E>
    struct makeStream_t 
    {
        template<class F>
        auto operator()(F && factory) const
            -> detail::SourceStream<std::remove_cvref_t<F>, T, E>
        {
            return {static_cast<F&&>(factory)};
        }
    };
    
    /**
     * @brief Construct a stream 
     * 
     * @tparam T Value type
     * @tparam E Error type
     */
    template<class T, class E>
    inline constexpr makeStream_t<T, E> makeStream{};
}