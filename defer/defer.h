#pragma once
#include <stack>

template <typename Callback>
class CallbackStorage {
public:
    explicit CallbackStorage(Callback callback) {
        new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }

    ~CallbackStorage() {
        if (alive_) {
            reinterpret_cast<Callback*>(callback_buffer_)->~Callback();
            alive_ = false;
        }
    }

private:
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
    bool alive_ = true;
};

template <typename Callback>
class Defer final {
public:
    Defer(Callback cb) : storage_(std::move(cb)) {
    }

    ~Defer() {
        if (alive_) {
            std::move(storage_.GetCallback())();
            alive_ = false;
        }
    }

    void Invoke() {
        if (alive_) {
            std::move(storage_.GetCallback())();
            Cancel();
        }
    }

    void Cancel() {
        if (alive_) {
            storage_.~CallbackStorage();
            alive_ = false;
        }
    }

private:
    CallbackStorage<Callback> storage_;
    bool alive_ = true;
};
