// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/JsonPrinter.hpp"

namespace signal_estimator {

JsonPrinter::JsonPrinter(Console& console)
    : console_(console) {
}

JsonPrinter::~JsonPrinter() {
    if (!first_output_) {
        console_.write("\n]\n");
    }
}

void JsonPrinter::println_(const char* str) {
    if (first_output_) {
        first_output_ = false;
        console_.write("[\n");
    } else {
        console_.write(",\n");
    }

    console_.write(str);
    console_.flush();
}

} // namespace signal_estimator
