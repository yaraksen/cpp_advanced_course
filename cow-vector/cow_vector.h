#pragma once
#include <vector>
#include <string>

struct State {
    int ref_count = 1;
    std::vector<std::string> vec;

    void Plus() {
        ++ref_count;
    }
    void Minus() {
        --ref_count;
    }
};

class COWVector {
public:
    COWVector() {
        state_ = new State;
    };

    ~COWVector() {
        if (state_->ref_count <= 1) {
            delete state_;
        } else {
            state_->Minus();
        }
    };

    COWVector(const COWVector& other) {
        state_ = other.state_;
        state_->Plus();
    };
    COWVector& operator=(const COWVector& other) {
        state_->Minus();
        state_ = other.state_;
        state_->Plus();
        return *this;
    };

    size_t Size() const {
        return state_->vec.size();
    }

    void Resize(size_t size) {
        DeepCopy();
        state_->vec.resize(size);
    }

    const std::string& Get(size_t at) const {
        return state_->vec.at(at);
    };
    const std::string& Back() const {
        return state_->vec.back();
    }

    void PushBack(const std::string& value) {
        DeepCopy();
        state_->vec.push_back(value);
    }

    void Set(size_t at, const std::string& value) {
        DeepCopy();
        state_->vec[at] = value;
    }

    void DeepCopy() {
        if (this->state_->ref_count > 1) {
            auto other = this->state_->vec;
            this->state_->Minus();
            this->state_ = new State();
            this->state_->vec = std::move(other);
        }
    }

private:
    State* state_ = nullptr;
};