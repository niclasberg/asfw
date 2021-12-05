#pragma once

namespace tmp
{
    template<template<typename...> class F>
    struct lift_
    {
        template<class... Args>
        using apply = F<Args...>;
    };
}