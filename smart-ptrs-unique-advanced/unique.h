#pragma once

#include "compressed_pair.h"
#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

// Primary template
template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    UniquePtr() noexcept : data_(nullptr, Deleter()){};

    template <typename U>
    explicit UniquePtr(U* ptr = nullptr) noexcept : data_(ptr, Deleter()){};

    //template <typename U>
    UniquePtr(T* ptr, Deleter deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)){};

    // Move Constructors

    template <typename Up, typename D>
    UniquePtr(UniquePtr<Up, D>&& other) noexcept {
        data_.GetFirst() = other.data_.GetFirst();
        data_.GetSecond() = std::forward<D>(other.data_.GetSecond());
        other.data_.GetFirst() = nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (data_.GetFirst() != other.data_.GetFirst()) {
            if (data_.GetFirst()) {
                data_.GetSecond()(data_.GetFirst());
            }
            data_.GetFirst() = other.data_.GetFirst();
            data_.GetSecond() = std::forward<Deleter>(other.data_.GetSecond());
            other.data_.GetFirst() = nullptr;
        }
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (data_.GetFirst()) {
            data_.GetSecond()(data_.GetFirst());
        }
        data_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (data_.GetFirst() != nullptr) {
            data_.GetSecond()(data_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        T* p = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return p;
    }
    void Reset(T* ptr = nullptr) noexcept {
        if (data_.GetFirst() != ptr) {
            auto old_pointer = data_.GetFirst();
            data_.GetFirst() = ptr;
            data_.GetSecond()(old_pointer);
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return data_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return data_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return *(data_.GetFirst());
    }
    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_;

    template <typename Up, typename Deleter2>
    friend class UniquePtr;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    UniquePtr() noexcept : data_(nullptr, Deleter()){};

    template <typename U>
    explicit UniquePtr(U* ptr = nullptr) noexcept : data_(ptr, Deleter()){};

    //template <typename U>
    UniquePtr(T* ptr, Deleter deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)){};

    // Move Constructors

    template <typename Up, typename D>
    UniquePtr(UniquePtr<Up, D>&& other) noexcept {
        data_.GetFirst() = other.data_.GetFirst();
        data_.GetSecond() = std::forward<D>(other.data_.GetSecond());
        other.data_.GetFirst() = nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (data_.GetFirst() != other.data_.GetFirst()) {
            if (data_.GetFirst()) {
                data_.GetSecond()(data_.GetFirst());
            }
            data_.GetFirst() = other.data_.GetFirst();
            data_.GetSecond() = std::forward<Deleter>(other.data_.GetSecond());
            other.data_.GetFirst() = nullptr;
        }
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (data_.GetFirst()) {
            data_.GetSecond()(data_.GetFirst());
        }
        data_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (data_.GetFirst() != nullptr) {
            data_.GetSecond()(data_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        T* p = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return p;
    }
    void Reset(T* ptr = nullptr) noexcept {
        if (data_.GetFirst() != ptr) {
            auto old_pointer = data_.GetFirst();
            data_.GetFirst() = ptr;
            data_.GetSecond()(old_pointer);
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return data_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return data_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator[](size_t ind) {
        return data_.GetFirst()[ind];
    }

private:
    CompressedPair<T*, Deleter> data_;

    template <typename Up, typename Deleter2>
    friend class UniquePtr;
};
