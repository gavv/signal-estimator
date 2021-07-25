/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "RunMaxCounter.hpp"

#include <algorithm>
#include <cassert>

namespace signal_estimator {

RunMaxCounter::Stack::Stack(size_t size)
    : items_(size) {
}

void RunMaxCounter::Stack::push(double val) {
    StackItem item;
    item.val = val;
    item.max = size_ == 0 ? val : std::max(get_max(), val);

    assert(size_ < items_.size());
    items_[size_++] = item;
}

double RunMaxCounter::Stack::pop() {
    assert(size_ > 0);
    return items_[--size_].val;
}

bool RunMaxCounter::Stack::is_empty() const {
    return size_ == 0;
}

double RunMaxCounter::Stack::get_max() const {
    assert(size_ > 0);
    return items_[size_ - 1].max;
}

RunMaxCounter::RunMaxCounter(size_t size)
    : push_stack_(size)
    , pop_stack_(size)
    , preload_(size) {
}

double RunMaxCounter::add(double val) {
    if (preload_ == 0) {
        pop_();
    } else {
        preload_--;
    }

    push_stack_.push(val);

    return get_max_();
}

void RunMaxCounter::pop_() {
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

double RunMaxCounter::get_max_() const {
    if (push_stack_.is_empty()) {
        return pop_stack_.get_max();
    }

    if (pop_stack_.is_empty()) {
        return push_stack_.get_max();
    }

    return std::max(push_stack_.get_max(), pop_stack_.get_max());
}

} // namespace signal_estimator
