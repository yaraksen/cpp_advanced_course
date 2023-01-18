#pragma once
#include <memory>
#include <vector>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual bool Do() = 0;
    virtual void Undo() = 0;
};

class TypeCommand : public ICommand {
public:
    TypeCommand(std::string& buffer, size_t& current_index, const char c)
        : current_index_(current_index), buffer_(buffer), c_(c) {
    }

    virtual bool Do() override {
        buffer_.insert(current_index_, 1, c_);
        ++current_index_;
        return true;
    }
    virtual void Undo() override {
        buffer_.erase(current_index_ - 1, 1);
        --current_index_;
    }

private:
    size_t& current_index_;
    std::string& buffer_;
    const char c_;
};

class ShiftLeftCommand : public ICommand {
public:
    ShiftLeftCommand(std::string& buffer, size_t& current_index)
        : buffer_(buffer), current_index_(current_index) {
    }

    virtual bool Do() override {
        if (current_index_ > 0) {
            complete_ = true;
            --current_index_;
            return true;
        } else {
            return false;
        }
    }
    virtual void Undo() override {
        if (complete_ && current_index_ < buffer_.size()) {
            ++current_index_;
        }
    }

private:
    bool complete_ = false;
    std::string& buffer_;
    size_t& current_index_;
};

class ShiftRightCommand : public ICommand {
public:
    ShiftRightCommand(std::string& buffer, size_t& current_index)
        : buffer_(buffer), current_index_(current_index) {
    }

    virtual bool Do() override {
        if (current_index_ < buffer_.size()) {
            complete_ = true;
            ++current_index_;
            return true;
        }
        return false;
    }
    virtual void Undo() override {
        if (complete_ && current_index_ > 0) {
            --current_index_;
        }
    }

private:
    bool complete_ = false;
    std::string& buffer_;
    size_t& current_index_;
};

class BackspaceCommand : public ICommand {
public:
    BackspaceCommand(std::string& buffer, size_t& current_index)
        : buffer_(buffer), current_index_(current_index) {
    }

    virtual bool Do() override {
        if (current_index_ > 0) {
            deleted_char_ = buffer_[current_index_ - 1];
            buffer_.erase(current_index_ - 1, 1);
            --current_index_;
            return true;
        }
        return false;
    }
    virtual void Undo() override {
        buffer_.insert(current_index_, 1, deleted_char_);
        ++current_index_;
    }

private:
    std::string& buffer_;
    char deleted_char_;
    size_t& current_index_;
};

class Editor {
public:
    const std::string& GetText() const {
        throw std::runtime_error("Not implemented");
    }

    void Type(char c) {
        AppendCommand(std::make_unique<TypeCommand>(buffer_, current_index_, c));
        history_point_ = history_.size() - 1;
    }

    void ShiftLeft() {
        AppendCommand(std::make_unique<ShiftLeftCommand>(buffer_, current_index_));
        history_point_ = history_.size() - 1;
    }

    void ShiftRight() {
        AppendCommand(std::make_unique<ShiftRightCommand>(buffer_, current_index_));
        history_point_ = history_.size() - 1;
    }

    void Backspace() {
        AppendCommand(std::make_unique<BackspaceCommand>(buffer_, current_index_));
        history_point_ = history_.size() - 1;
    }

    void Undo() {
        if (history_point_ >= 0) {
            history_[history_point_]->Undo();
            --history_point_;
        }
    }

    void Redo() {
        if (history_point_ < history_.size() - 1) {
            history_[history_point_ + 1]->Do();
            ++history_point_;
        }
    }

    const std::string& GetText() {
        return buffer_;
    }

private:
    std::string buffer_;
    size_t current_index_ = 0;
    int history_point_ = -1;
    std::vector<std::unique_ptr<ICommand>> history_;

    void AppendCommand(std::unique_ptr<ICommand> command) {
        history_.push_back(std::move(command));
        if (!history_.back()->Do()) {
            history_.pop_back();
        }
    }
};
