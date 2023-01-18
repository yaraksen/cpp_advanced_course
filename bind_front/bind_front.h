#pragma once

#include <functional>
#include <utility>

template <class F, class... Args>
constexpr auto BindFront(F&& f, Args&&... args) {
    return [... args = std::forward<Args>(args),
            f = std::forward<F>(f) ]<class... Args2>(Args2 && ... args2) mutable {
        return f(std::forward<decltype(args)>(args)..., std::forward<Args2>(args2)...);
    };
}
