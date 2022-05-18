// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

#include <array>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>

namespace signal_estimator {

template <typename T, size_t DEPTH> class Queue {
public:
    Queue()
        : head_(0)
        , tail_(0) {
    }

    Queue(const Queue<T, DEPTH>&) = delete;
    Queue& operator=(Queue<T, DEPTH>&) = delete;

    void push(const T x) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const ssize_t free_size = DEPTH - (tail - head_.load(std::memory_order_relaxed));
        assert(free_size > 0);
        if (free_size == 0) {
            return;
        }

        const size_t i = tail % DEPTH;
        buff_[i] = x;

        {
            std::lock_guard lk(mtx_);
            tail_++;
        }
        cv_.notify_one();
    }

    std::optional<T> pop(const bool block = false) {
        const size_t cur_head = head_.load(std::memory_order_relaxed);
        const ssize_t occupied_sz = tail_.load(std::memory_order_relaxed) - cur_head;
        assert(occupied_sz >= 0);
        if (occupied_sz == 0) {
            if (!block) {
                return {};
            } else {
                std::unique_lock<std::mutex> lck(mtx_);
                // Against spurious wakeup.
                cv_.wait(lck, [this] { return this->free_size() < DEPTH; });
            }
        }
        const size_t i = cur_head % buff_.size();
        T res = buff_[i];

        head_++;
        return res;
    }

    inline size_t free_size() {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_relaxed);
        return DEPTH - (tail - head);
    }

private:
    std::array<T, DEPTH> buff_;
    std::atomic<size_t> head_, tail_;

    std::mutex mtx_;
    std::condition_variable cv_;
};

} // namespace signal_estimator
