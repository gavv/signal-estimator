// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/TextPrinter.hpp"

namespace signal_estimator {

TextPrinter::TextPrinter(Console& console)
    : console_(console) {
}

void TextPrinter::println_(const char* str) {
    console_.write(str);
    console_.write("\n");
    console_.flush();
}

} // namespace signal_estimator
