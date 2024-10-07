// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/ConsoleSink.hpp"

#include <spdlog/fmt/bundled/format.h>

#include <mutex>

namespace signal_estimator {

template <typename Sink> class BasicTextPrinter final {
public:
    explicit BasicTextPrinter(Sink& sink)
        : sink_(sink) {
    }

    BasicTextPrinter(const BasicTextPrinter&) = delete;
    BasicTextPrinter& operator=(const BasicTextPrinter&) = delete;

    template <typename... Args>
    void write(const fmt::format_string<Args...>& fmt, Args&&... args) {
        std::unique_lock lock(mutex_);
        sink_.write(fmt, std::forward<Args>(args)...);
    }

private:
    Sink& sink_;
    std::mutex mutex_;
};

using TextPrinter = BasicTextPrinter<ConsoleSink>;

} // namespace signal_estimator
