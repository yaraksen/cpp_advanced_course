#pragma once

#include <exception>
#include <stdexcept>
#include <cstring>

template <class T>
class Try {
public:
    Try() = default;
    Try(T value) : value_(value), stored_value_(true){};
    const T& Value() const {
        if (reason_) {
            std::rethrow_exception(reason_);
        } else if (stored_value_) {
            return value_;
        } else {
            throw std::logic_error("Object is empty");
        }
    }

    template <typename Exception>
    Try(const Exception& exception) : reason_(std::make_exception_ptr(exception)){};
    Try(const std::exception_ptr& exception) : reason_(exception){};

    void Throw() {
        if (reason_) {
            std::rethrow_exception(reason_);
        } else {
            throw std::logic_error("No exception");
        }
    }

    bool IsFailed() const {
        return bool(reason_);
    }

private:
    T value_;
    bool stored_value_ = false;
    std::exception_ptr reason_;
};

template <>
class Try<void> {
public:
    Try() = default;
    template <typename Exception>
    Try(const Exception& exception) : reason_(std::make_exception_ptr(exception)){};
    Try(const std::exception_ptr& exception) : reason_(exception){};

    void Throw() {
        if (reason_) {
            std::rethrow_exception(reason_);
        } else {
            throw std::logic_error("No exception");
        }
    }

    bool IsFailed() const {
        return bool(reason_);
    }

private:
    std::exception_ptr reason_;
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));
    try {
        if constexpr (std::is_void_v<ReturnType>) {
            func(args...);
            return Try<ReturnType>();
        } else {
            return Try<ReturnType>(func(args...));
        }
    } catch (const std::exception& e) {
        return Try<ReturnType>(std::current_exception());
    } catch (const char* str) {
        return Try<ReturnType>(std::logic_error(str));
    } catch (int x) {
        return Try<ReturnType>(std::logic_error(std::strerror(x)));
    } catch (...) {
        return Try<ReturnType>(std::logic_error("Unknown exception"));
    }
}
