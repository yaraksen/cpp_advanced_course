#pragma once

#include "sw_fwd.h"  // Forward declaration
#include "shared.h"
#include <iostream>

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() : constructed_from_(nullptr), block_(nullptr) {
    }

    template <typename F>
    WeakPtr(const WeakPtr<F>& other)
        : constructed_from_(other.constructed_from_), block_(other.block_) {
        Increase();
    }
    WeakPtr(const WeakPtr& other)
        : constructed_from_(other.constructed_from_), block_(other.block_) {
        Increase();
    }

    template <typename F>
    WeakPtr(WeakPtr<F>&& other) : constructed_from_(other.constructed_from_), block_(other.block_) {
        other.block_ = nullptr;
        other.constructed_from_ = nullptr;
    }
    WeakPtr(WeakPtr&& other) : constructed_from_(other.constructed_from_), block_(other.block_) {
        other.block_ = nullptr;
        other.constructed_from_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    template <typename U>
    WeakPtr(const SharedPtr<U>& other) : constructed_from_(other.ptr_), block_(other.block_) {
        Increase();
    }
    WeakPtr(const SharedPtr<T>& other) : constructed_from_(other.ptr_), block_(other.block_) {
        Increase();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <typename F>
    WeakPtr& operator=(const WeakPtr<F>& other) {
        if (constructed_from_ != other.constructed_from_) {
            Decrease();
            constructed_from_ = other.constructed_from_;
            block_ = other.block_;
            Increase();
        }
        return *this;
    }
    WeakPtr& operator=(const WeakPtr& other) {
        if (constructed_from_ != other.constructed_from_) {
            Decrease();
            constructed_from_ = other.constructed_from_;
            block_ = other.block_;
            Increase();
        }
        return *this;
    }
    template <typename F>
    WeakPtr& operator=(WeakPtr<F>&& other) {
        if (constructed_from_ != other.constructed_from_) {
            Decrease();
            constructed_from_ = other.constructed_from_;
            block_ = other.block_;
            other.constructed_from_ = nullptr;
            other.block_ = nullptr;
        }
        return *this;
    }
    WeakPtr& operator=(WeakPtr&& other) {
        if (constructed_from_ != other.constructed_from_) {
            Decrease();
            constructed_from_ = other.constructed_from_;
            block_ = other.block_;
            other.constructed_from_ = nullptr;
            other.block_ = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        Decrease();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        Decrease();
        constructed_from_ = nullptr;
        block_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        auto temp_block = other.block_;
        auto temp_constructed_from = other.constructed_from_;
        other.block_ = block_;
        other.constructed_from_ = constructed_from_;
        constructed_from_ = temp_constructed_from;
        block_ = temp_block;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (block_) {
            return block_->reference_counter;
        } else {
            return 0;
        }
    }
    bool Expired() const {
        return (UseCount() == 0);
    }
    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

private:
    T* constructed_from_{};
    ControlBlockBase* block_{};

    void Decrease() noexcept {
        if (block_ != nullptr) {
            if (block_->weak_reference_counter == 1) {
                if (block_->reference_counter == 0) {
                    --block_->weak_reference_counter;
                    delete block_;
                    block_ = nullptr;
                } else {
                    --block_->weak_reference_counter;
                }
            } else if (block_->weak_reference_counter > 1) {
                --block_->weak_reference_counter;
            }
        }
    }

    void Increase() noexcept {
        if (block_ != nullptr) {
            ++block_->weak_reference_counter;
        }
    }

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

    template <typename U>
    friend class EnableSharedFromThis;

    friend class EnableSharedFromThisBase;
};