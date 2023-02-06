#pragma once
#include "stream.hpp"
#include <type_traits>
#include <tuple>

namespace async
{
    namespace detail
    {
        template<class ... Streams>
        class ConcatStream
        {
        public:

        private:
            std::tuple<Streams...> streams_;
        };
    }

    inline constexpr struct concat_t final
    {
        template<AnyStream ... Streams>
        auto operator()(Streams && ... streams) const
            -> detail::ConcatStream<std::remove_cvref_t<Streams>...>
        {

        }
    } concat {};
}