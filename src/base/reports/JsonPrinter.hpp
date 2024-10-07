// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/Console.hpp"

#include <spdlog/fmt/bundled/format.h>

#include <mutex>
#include <utility>

namespace signal_estimator {

class JsonPrinter final {
public:
    explicit JsonPrinter(Console& console);
    ~JsonPrinter();

    JsonPrinter(const JsonPrinter&) = delete;
    JsonPrinter& operator=(const JsonPrinter&) = delete;

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
    bool first_output_ { true };
    char fmt_buf_[512];
    std::mutex mutex_;
};

} // namespace signal_estimator
