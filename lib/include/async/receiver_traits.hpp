#pragma once
#include <type_traits>

namespace async
{
    namespace detail
    {
        template<class Receiver, typename Type, class = void>
        struct HasSetNextImpl : std::false_type { };

        template<class Receiver, class ... Types>
        struct HasSetNextImpl<
            Receiver, 
            async::helpers::TypeList<Types...>, 
            decltype(std::declval<Receiver>().setNext(std::declval<Types>()...), void())> 
        : std::true_type { };

        template<class Receiver>
        struct HasSetNextImpl<
            Receiver, 
            async::helpers::TypeList<void>, 
            decltype(std::declval<Receiver>().setNext(), void())> 
        : std::true_type { };
    }

    template<class Receiver, class ... Ts>
    struct IsReceiverOf : detail::HasSetNextImpl<Receiver, helpers::TypeList<Ts...>> 
    {

    };
    
    template<class Receiver, class = void> 
    struct IsDoneReceiver : std::false_type {};

    template<class Receiver>
    struct IsDoneReceiver<
        Receiver,
        decltype(std::declval<Receiver>().setDone(), void())>
    : std::true_type {};
}