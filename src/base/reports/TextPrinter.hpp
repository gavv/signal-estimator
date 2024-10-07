// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/Console.hpp"

#include <fmt/format.h>

#include <mutex>
#include <utility>

namespace signal_estimator {

class TextPrinter final {
public:
    explicit TextPrinter(Console& console);

    TextPrinter(const TextPrinter&) = delete;
    TextPrinter& operator=(const TextPrinter&) = delete;

    template <typename... Args>
    void println(const fmt::format_string<Args...>& fmt, Args&&... args) {
        std::unique_lock lock(mutex_);

        auto result = fmt::format_to_n(
            fmt_buf_, sizeof(fmt_buf_), fmt, std::forward<Args>(args)...);
        *result.out = '\0';

        println_(fmt_buf_);
    }

private:
    void println_(const char* str);

    Console& console_;
    char fmt_buf_[512];
    std::mutex mutex_;
};

} // namespace signal_estimator
