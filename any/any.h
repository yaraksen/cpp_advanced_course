#pragma once
#include <memory>

class Any {
public:
    class InnerBase {
    public:
        virtual ~InnerBase(){};
        virtual InnerBase* Clone() const = 0;
    };

    template <class T>
    class Inner : public InnerBase {
    public:
        Inner(const T& val) : val_(val){};
        virtual InnerBase* Clone() const override {
            return new Inner<T>(val_);
        }

    private:
        T val_;

        friend class Any;
    };

    Any() : ptr_(nullptr) {
    }

    template <class T>
    Any(const T& value) : ptr_(std::make_unique<Inner<T>>(std::move(value))) {
    }

    template <class T>
    Any& operator=(const T& value) {
        ptr_.reset(new Inner<T>(std::move(value)));
        return *this;
    }

    Any(const Any& rhs) {
        if (ptr_ != rhs.ptr_) {
            ptr_.reset(rhs.ptr_->Clone());
        }
    }

    Any& operator=(const Any& rhs) {
        if (ptr_ != rhs.ptr_) {
            ptr_.reset(rhs.ptr_->Clone());
        }
        return *this;
    }
    ~Any() {
    }

    bool Empty() const {
        return !ptr_.get();
    }

    void Clear() {
        ptr_.reset(nullptr);
    }
    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() {
        return dynamic_cast<Inner<T>&>(*ptr_).val_;
    }

private:
    std::unique_ptr<InnerBase> ptr_ = nullptr;
};