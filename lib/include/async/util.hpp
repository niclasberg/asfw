#pragma once
#include "future.hpp"
#include "tmp/type_list.hpp"

namespace async
{
    namespace detail
    {
        template<class TList> struct extractErrorType_;
        template<class T> 
        struct extractErrorType_<tmp::TypeList<T>> { using type = T; };

        template<class T> 
        struct extractErrorType_<tmp::TypeList<T, void>> { using type = T; };

        template<class T> 
        struct extractErrorType_<tmp::TypeList<void, T>> { using type = T; };
    }

    template<AnyFuture ... Ss>
    using combined_future_error_t = typename 
        detail::extractErrorType_<
            tmp::unique_<tmp::TypeList<future_error_t<Ss>...>>
        >::type;

    template<class ... Ss>
    concept HasCombinableErrors = requires {
        typename combined_future_error_t<Ss...>;
    };
}