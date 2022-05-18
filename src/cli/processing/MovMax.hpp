// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdlib>
#include <vector>

namespace signal_estimator {

// running maximum with amortized O(1) push
// see https://stackoverflow.com/a/4802260/3169754
template <typename T = double, bool preload = true> class MovMax {
public:
    explicit MovMax(size_t size)
        : push_stack_(size)
        , pop_stack_(size)
        , preload_(size) {
        if (!preload) {
            preload_ = 0;
            for (size_t i = 0; i < size; ++i) {
                push_stack_.push(T(0));
            }
        }
    }

    MovMax(const MovMax&) = delete;
    MovMax& operator=(const MovMax&) = delete;

    T add(T val) {
        if (preload_ == 0) {
            pop_();
        } else {
            preload_--;
        }

        push_stack_.push(val);

        return get_max_();
    }

private:
    struct StackItem {
        T val {};
        T max {};
    };

    class Stack {
    public:
        explicit Stack(size_t size)
            : items_(size)
            , size_(0) {
        }

        void push(T val) {
            StackItem item;
            item.val = val;
            item.max = size_ == 0 ? val : std::max(get_max(), val);

            assert(size_ < items_.size());
            items_[size_++] = item;
        }

        T pop() {
            assert(size_ > 0);
            return items_[--size_].val;
        }

        bool is_empty() const {
            return size_ == 0;
        }

        T get_max() const {
            assert(size_ > 0);
            return items_[size_ - 1].max;
        }

    private:
        std::vector<StackItem> items_;
        size_t size_ {};
    };

    void pop_() {
        if (pop_stack_.is_empty()) {
            for (;;) {
                const auto val = push_stack_.pop();
                if (push_stack_.is_empty()) {
                    break;
                }
                pop_stack_.push(val);
            }
        } else {
            pop_stack_.pop();
        }
    }
    T get_max_() const {
        if (push_stack_.is_empty()) {
            return pop_stack_.get_max();
        }

        if (pop_stack_.is_empty()) {
            return push_stack_.get_max();
        }

        return std::max(push_stack_.get_max(), pop_stack_.get_max());
    }

    Stack push_stack_;
    Stack pop_stack_;

    size_t preload_ { 0 };
};

} // namespace signal_estimator
