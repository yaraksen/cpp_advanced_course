#pragma once

#include <atomic>
#include <optional>
#include <stdexcept>
#include <utility>
#include <list>
#include <unordered_map>
#include <thread>

template <class T>
struct Node {
    T value;
    Node* next;
};

template <class T>
class MPSCStack {
public:
    // Push adds one element to stack top.
    //
    // Safe to call from multiple threads.
    void Push(const T& value) {
        auto new_head = new Node<T>{value, head_.load()};
        while (!head_.compare_exchange_weak(new_head->next, new_head)) {
        }
    }

    // Pop removes top element from the stack.
    //
    // Not safe to call concurrently.
    std::optional<T> Pop() {
        while (true) {
            Node<T>* head = head_.load();
            if (!head) {
                return std::nullopt;
            }
            /*if (head == head_.load()) {
                hazard_ptr_[std::this_thread::get_id()] =
                    head;  // saving current Node that we're deleting
            } else {
                continue;
            }*/

            while (head && !head_.compare_exchange_weak(head, head->next)) {
                head = head_.load();
            }
            if (!head) {
                return std::nullopt;
            }
            // hazard_ptr_[std::this_thread::get_id()] = nullptr;
            // to_free_list_.push_back(head);
            T value = std::move(head->value);
            delete head;
            return value;
        }
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        Node<T>*head, *next;
        while ((head = head_.exchange(nullptr))) {
            while (head) {
                next = head->next;
                cb(head->value);
                delete head;
                head = next;
            }
        }
    }

    ~MPSCStack() {
        Node<T>*head, *next;
        while ((head = head_.exchange(nullptr))) {
            while (head) {
                next = head->next;
                delete head;
                head = next;
            }
        }
        /*for (auto it = to_free_list_.begin(); it != to_free_list_.end(); ++it) {
            delete *it;
        }*/
        /*while (!to_free_list_.empty()) {
            for (auto it = to_free_list_.begin(); it != to_free_list_.end(); ++it) {
                bool flag = true;
                for (auto it2 = hazard_ptr_.begin(); it2 != hazard_ptr_.end(); ++it2) {
                    if (it2->second == *it) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    delete *it;
                    to_free_list_.erase(it);
                    --it;
                }
            }
        }*/
    }

private:
    // std::list<Node<T>*> to_free_list_;
    // std::unordered_map<std::thread::id, Node<T>*> hazard_ptr_;  // vector of pointers to Nodes
    // that are currently used in other threads
    std::atomic<Node<T>*> head_ = nullptr;
    // Node<T>* nonatomic_head_ = nullptr;
};
