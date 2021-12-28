#pragma once
#include "sender.hpp"
#include "tmp/type_list.hpp"

namespace async
{
    template<template<typename ...> typename Variant, class ... Senders>
    using CombinedSenderErrorTypes = tmp::apply_<
        tmp::unique_<
            tmp::concat_<
                SenderErrorTypes<Senders, tmp::TypeList>...>>, 
        Variant>;

    template<template<typename ...> typename Variant, class ... Senders>
    using CombinedSenderSignalTypes = tmp::apply_<
        tmp::unique_<
            tmp::concat_<
                SenderSignalTypes<Senders, tmp::TypeList>...>>, 
        Variant>;
}