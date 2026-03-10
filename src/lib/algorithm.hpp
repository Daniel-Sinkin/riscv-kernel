
// lib/algorithm.hpp

#pragma once

#include <concepts>
#include <limits>

namespace lib
{

template <std::totally_ordered T>
constexpr auto max(T x, T y) -> T
{
    return (x < y) ? y : x;
}

template <std::totally_ordered T>
constexpr auto min(T x, T y) -> T
{
    return (x < y) ? x : y;
}

template <std::signed_integral I>
constexpr auto abs(I x) -> I
{
    if (x == std::numeric_limits<I>::lowest())
    {
        // There is no good way to handle INT_MIN for std::abs
        // Just going to pass back the negative nubmer
        // to signal that abs failed
        return x;
    }
    return (x < 0) ? -x : x;
}

template <std::unsigned_integral I>
constexpr auto abs(I x) -> I
{
    return x;
}

}  // namespace lib
