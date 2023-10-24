// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <mutex>

namespace signal_estimator {

class JsonPrinter {
public:
    JsonPrinter();
    ~JsonPrinter();

    JsonPrinter(const JsonPrinter&) = delete;
    JsonPrinter& operator=(const JsonPrinter&) = delete;

    __attribute__((format(printf, 2, 3))) void write_line(const char* fmt, ...);

private:
    std::mutex mutex_;
    bool first_output_;
};

} // namespace signal_estimator
