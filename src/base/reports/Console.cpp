// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/Console.hpp"

#include <cassert>
#include <cstdio>

namespace signal_estimator {

void Console::write(const char* str) {
    assert(str);

    fprintf(stdout, "%s", str);
}

void Console::flush() {
    fflush(stdout);
}

} // namespace signal_estimator
