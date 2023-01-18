#pragma once

#include <utility>
#include <tuple>

template <class F>
constexpr auto Curry(F&& f) {
    return [f = std::forward<F>(f)]<class... Args>(Args && ... args) mutable {
        return f(std::tuple(std::forward<Args>(args)...));
    };
}

template <class F>
constexpr auto Uncurry(F&& f) {
    return [f = std::forward<F>(f)]<typename T>(T&& tuple) mutable {
        return std::apply(std::forward<decltype(f)>(f), std::forward<T>(tuple));
    };
}
