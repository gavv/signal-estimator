// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/JsonPrinter.hpp"

#include <cstdarg>
#include <cstdio>

namespace signal_estimator {

JsonPrinter::JsonPrinter() {
    first_output_ = true;
}

JsonPrinter::~JsonPrinter() {
    if (!first_output_) {
        printf("\n]\n");
    }
}

void JsonPrinter::write_line(const char* fmt, ...) {
    std::lock_guard lock(mutex_);

    if (first_output_) {
        first_output_ = false;
        printf("[\n");
    } else {
        printf(",\n");
    }

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

} // namespace signal_estimator
