#pragma once
#include <array>
//make array filled with value
template<typename T, size_t N>
constexpr auto make_array(T value);










template<typename T, size_t N>
constexpr auto make_array(T value)
{
    auto a = std::array<T, N>{};
    for (auto& x : a)
        x = value;
    return a;
}