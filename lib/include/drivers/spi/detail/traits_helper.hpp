#pragma once

namespace drivers::spi::detail
{
    template<class T> concept hasOnStop = requires { T::onStop(); };
    template<class T> concept hasOnStart = requires { T::onStart(); };
    template<class T> concept hasOnEvent = requires { T::onEvent(); };
}