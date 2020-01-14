/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <vector>

namespace signal_estimator {

// amortized O(1) running maximum
// https://stackoverflow.com/a/4802260/3169754
class RunMaxCounter {
public:
    RunMaxCounter(size_t size);

    RunMaxCounter(const RunMaxCounter&) = delete;
    RunMaxCounter& operator=(const RunMaxCounter&) = delete;

    double add(double);

private:
    struct StackItem {
        double val {};
        double max {};
    };

    class Stack {
    public:
        Stack(size_t);

        void push(double);
        double pop();

        bool is_empty() const;
        double get_max() const;

    private:
        std::vector<StackItem> items_;
        size_t size_ {};
    };

    void pop_();
    double get_max_() const;

    Stack push_stack_;
    Stack pop_stack_;

    size_t preload_ { 0 };
};

} // namespace signal_estimator
