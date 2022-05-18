// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <assert.h>

namespace signal_estimator {

class ListNode {
public:
    ListNode()
        : next_(nullptr) {
    }

    ListNode* next() const {
        return next_;
    }

    void push(ListNode** head) {
        assert(!next_);
        next_ = *head;
        *head = this;
    }

    static ListNode* pop(ListNode** head) {
        ListNode* res = *head;
        *head = (*head)->next_;
        res->next_ = nullptr;
        return res;
    }

protected:
    ListNode* next_;
};

} // namespace signal_estimator
