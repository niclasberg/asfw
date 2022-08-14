#pragma once

template<class T>
struct ReadSpan
{
    volatile T * data;
    std::uint32_t size;
};