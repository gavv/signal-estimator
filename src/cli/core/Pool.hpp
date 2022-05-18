// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/List.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

namespace signal_estimator {

template <typename T> class Pool {
public:
    Pool(const Config& config)
        : config_(config) {
        pool_.reserve(n_);
        for (size_t i = 0; i < n_; ++i) {
            pool_.emplace(pool_.end(), config, this);
        }

        free_list_head_ = nullptr;
        for (auto& x : pool_) {
            ListNode* p((ListNode*)&x);
            p->push(&free_list_head_);
        }
    }

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

    T* allocate() {
        std::lock_guard<std::mutex> guard(mutex_);
        auto* p = ListNode::pop(&free_list_head_);
        return (T*)p;
    }

    void deallocate(T* f) {
        std::lock_guard<std::mutex> guard(mutex_);
        f->push(&free_list_head_);
    }

private:
    static constexpr size_t n_ = 128;
    std::vector<T> pool_;
    ListNode* free_list_head_;

    const Config& config_;
    std::mutex mutex_;
};

} // namespace signal_estimator
