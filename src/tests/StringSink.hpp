// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <spdlog/fmt/bundled/format.h>

#include <string>

struct StringSink {
    template <typename... Args>
    void write(const fmt::format_string<Args...>& fmt, Args&&... args) {
        buffer = fmt::format(fmt, std::forward<Args>(args)...);
    }

    std::string buffer;
};
