#pragma once

#include <utility>

template <std::size_t N>
constexpr auto Fib() {
    if constexpr (N < 2) {
        return [] { return N; };
    } else {
        return [] { return Fib<N - 1>()() + Fib<N - 2>()(); };
    }
}

template <std::size_t N>
constexpr auto Fact() {
    if constexpr (N > 1) {
        return [] { return N * Fact<N - 1>()(); };
    } else if constexpr (N == 0) {
        return [] { return 1; };
    } else {
        return [] { return N; };
    }
}

template <std::size_t A, std::size_t B>
constexpr auto GCD() {
    if constexpr (B == 0) {
        return [] { return A; };
    } else {
        return [] { return GCD<B, A % B>()(); };
    }
}

template <std::size_t A, std::size_t H = 2>
constexpr auto Prime() -> bool (*)() {
    if (A <= 1) {
        return [] { return false; };
    } else if constexpr (H * H > A) {
        return [] { return true; };
    } else if constexpr (A % H == 0) {
        return [] { return false; };
    } else {
        return [] { return Prime<A, H + 1>()(); };
    }
}
