#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

template <class F, class... Args>
using TransformResult = std::tuple<typename std::invoke_result<F, Args>::type...>;  /// 1

template <class F, class... Types>
concept invocable_on_all_types = (std::is_invocable_v<F, Types> && ...);  /// 2

template <class F, class... Args>
constexpr TransformResult<F, Args...> TransformTuple(
    F&& functor, const std::tuple<Args...>& args) requires invocable_on_all_types<F, Args...> {
    return std::apply([&functor](auto... tuple) { return std::make_tuple(functor(tuple)...); },
                      args);  /// 3
}

template <size_t index = 0, class F, class... Args>
constexpr auto TransformReduceTuple(
    F&& functor, const std::tuple<Args...>& args) requires invocable_on_all_types<F, Args...> {
    if constexpr (index + 1 >= sizeof...(Args)) {
        return std::make_tuple(functor(std::get<index>(args)));
    } else {
        if constexpr (std::is_void_v<std::invoke_result_t<F, decltype(std::get<index>(args))>>) {
            return TransformReduceTuple<index + 1, F, Args...>(functor, args);
        } else {
            return std::tuple_cat(std::make_tuple(functor(std::get<index>(args))),
                                  TransformReduceTuple<index + 1, F, Args...>(functor, args));
        }  /// 4
    }
}
