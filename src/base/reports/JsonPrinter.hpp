// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/ConsoleSink.hpp"

#include <spdlog/fmt/bundled/format.h>

#include <mutex>

namespace signal_estimator {

template <typename Sink> class BasicJsonPrinter final {
public:
    explicit BasicJsonPrinter(Sink& sink)
        : sink_(sink) {
    }

    ~BasicJsonPrinter() {
        if (!first_output_) {
            sink_.write("{}", "\n]\n");
        }
    }

    BasicJsonPrinter(const BasicJsonPrinter&) = delete;
    BasicJsonPrinter& operator=(const BasicJsonPrinter&) = delete;

    template <typename... Args>
    void write(const fmt::format_string<Args...>& fmt, Args&&... args) {
        std::unique_lock lock(mutex_);

        if (first_output_) {
            first_output_ = false;
            sink_.write("{}", "[\n");
        } else {
            sink_.write("{}", ",\n");
        }

        sink_.write(fmt, std::forward<Args>(args)...);
    }

private:
    bool first_output_ { true };
    Sink& sink_;
    std::mutex mutex_;
};

using JsonPrinter = BasicJsonPrinter<ConsoleSink>;

} // namespace signal_estimator
