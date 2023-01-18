#pragma once

#include <type_traits>
#include <iostream>

// Me think, why waste time write lot code, when few code do trick.
template <typename F, int tag, bool CanApplyEBO = std::is_empty_v<F> && !std::is_final_v<F>>
class CompressedPairElement {
public:
    CompressedPairElement() : value_() {
    }
    template <typename T>
    CompressedPairElement(T&& value) : value_(std::forward<T>(value)) {
    }

    const F& Get() const {
        return value_;
    }
    F& Get() {
        return value_;
    }

private:
    F value_;
};

template <typename F, int tag>
class CompressedPairElement<F, tag, true> : public F {
public:
    CompressedPairElement() {
    }
    template <typename T>
    CompressedPairElement(T&&) {
    }

    const F& Get() const {
        return *this;
    }
    F& Get() {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;

public:
    CompressedPair() : First(), Second(){};
    template <typename T1, typename T2>
    CompressedPair(T1&& first, T2&& second)
        : First(std::forward<T1>(first)), Second(std::forward<T2>(second)){};

    F& GetFirst() {
        return First::Get();
    }

    const F& GetFirst() const {
        return First::Get();
    }

    S& GetSecond() {
        return Second::Get();
    };

    const S& GetSecond() const {
        return Second::Get();
    };
};
