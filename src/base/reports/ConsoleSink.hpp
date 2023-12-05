// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <spdlog/fmt/bundled/format.h>

namespace signal_estimator {

class ConsoleSink final {
public:
    template <typename... Args>
    void write(const fmt::format_string<Args...>& fmt, Args&&... args) {
        fmt::print(fmt, std::forward<Args>(args)...);
    }
};

} // namespace signal_estimator
