#pragma once

#include "sw_fwd.h"  // Forward declaration
#include <cstddef>   // std::nullptr_t
#include <iostream>
#include <type_traits>

class EnableSharedFromThisBase {};

template <class T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
public:
    SharedPtr<T> SharedFromThis() {
        return self_.Lock();
    }
    SharedPtr<const T> SharedFromThis() const {
        return self_.Lock();
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return self_;
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return self_;
    }

private:
    WeakPtr<T> self_;

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&... args);
};

class ControlBlockBase {
public:
    size_t reference_counter = 0, weak_reference_counter = 0;
    virtual ~ControlBlockBase() = default;
    virtual void DeleteOnlyPointer(){};
};

template <typename U>
class ControlBlockPointer : public ControlBlockBase {
public:
    ControlBlockPointer(U* ptr) : ptr_we_delete_(ptr){};

    ~ControlBlockPointer() noexcept {
        delete ptr_we_delete_;
        ptr_we_delete_ = nullptr;
    }

    void DeleteOnlyPointer() override {
        delete ptr_we_delete_;
        ptr_we_delete_ = nullptr;
    }

    U* ptr_we_delete_ = nullptr;  // pointer we delete
};

template <typename U>
class ControlBlockHolder : public ControlBlockBase {
public:
    template <typename... Args>
    ControlBlockHolder(Args&&... args) {
        new (&raw_memory_) U(std::forward<Args>(args)...);
        ++reference_counter;
    }

    U* GetRawPtr() {
        return reinterpret_cast<U*>(raw_memory_);
    }

    ~ControlBlockHolder() {
        reinterpret_cast<U*>(raw_memory_)->~U();
    }

    void DeleteOnlyPointer() override {
        reinterpret_cast<U*>(raw_memory_)->~U();
    }

private:
    std::aligned_storage<sizeof(U), alignof(U)> raw_memory_[sizeof(U)];
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() noexcept : ptr_(nullptr), block_(nullptr) {
    }
    SharedPtr(std::nullptr_t) noexcept : ptr_(nullptr), block_(nullptr) {
    }

    template <typename U>
    explicit SharedPtr(U* ptr) noexcept : ptr_(ptr), block_(new ControlBlockPointer<U>(ptr)) {
        if constexpr (std::is_convertible_v<U*, EnableSharedFromThisBase*>) {
            // if U is derived EnableSharedFromThis
            ptr->self_ = *this;
        }
        Increase();
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) : ptr_(other.ptr_), block_(other.block_) {
        Increase();
    }
    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        Increase();
    }

    template <typename F>
    SharedPtr(SharedPtr<F>&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr), block_(other.block_) {
        Increase();
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other)
        : ptr_(other.constructed_from_), block_(other.block_) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        Increase();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (ptr_ != other.ptr_) {
            Decrease();
            ptr_ = other.ptr_;
            block_ = other.block_;
            Increase();
        }
        return *this;
    }

    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) {
        if (ptr_ != other.ptr_) {
            Decrease();
            ptr_ = other.ptr_;
            block_ = other.block_;
            other.ptr_ = nullptr;
            other.block_ = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() noexcept {
        Decrease();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        Decrease();
        ptr_ = nullptr;
        block_ = nullptr;
    }
    template <typename U>
    void Reset(U* ptr) {
        Decrease();
        ptr_ = ptr;
        block_ = new ControlBlockPointer<U>(ptr);
        block_->reference_counter = 1;
    }
    void Swap(SharedPtr& other) {
        auto temp_block = other.block_;
        auto temp_ptr = other.ptr_;
        other.block_ = block_;
        other.ptr_ = ptr_;
        ptr_ = temp_ptr;
        block_ = temp_block;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (block_) {
            return block_->reference_counter;
        } else {
            return 0;
        }
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_{};

    void Decrease() noexcept {
        if (block_ != nullptr) {
            if (block_->reference_counter == 1) {
                if (block_->weak_reference_counter == 0) {
                    --block_->reference_counter;
                    delete block_;
                    block_ = nullptr;
                } else {
                    block_->DeleteOnlyPointer();
                    --block_->reference_counter;
                    if (block_->weak_reference_counter == 0) {
                        delete block_;
                        block_ = nullptr;
                    }
                }
            } else if (block_->reference_counter > 1) {
                --block_->reference_counter;
            }

        }
    }

    void Increase() noexcept {
        if (block_ != nullptr) {
            ++block_->reference_counter;
        }
    }

    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&... args);

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

    template <typename U>
    friend class EnableSharedFromThis;
};

template <typename F, typename U>
inline bool operator==(const SharedPtr<F>& left, const SharedPtr<U>& right) {
    return right.Get() == left.Get();
}

// Allocate memory only once
template <typename U, typename... Args>
SharedPtr<U> MakeShared(Args&&... args) {
    SharedPtr<U> maked;
    auto new_block = new ControlBlockHolder<U>(std::forward<Args>(args)...);
    maked.block_ = new_block;
    maked.ptr_ = new_block->GetRawPtr();
    if constexpr (std::is_convertible_v<U*, EnableSharedFromThisBase*>) {
        // if U is derived EnableSharedFromThis
        maked.ptr_->self_ = maked;
    }
    return maked;
};

// Look for usage examples in tests
