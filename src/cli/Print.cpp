// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Print.hpp"

#include "io/PcmFormat.hpp"

#include <algorithm>

namespace signal_estimator {

void print_supported_formats(std::ostream& out) {
    const auto formats = PcmFormat::supported_formats();

    out << "Device formats:\n";

    for (size_t bits = 0; bits <= 64; bits++) {
        for (size_t bytes = 0; bytes <= 8; bytes++) {
            for (bool is_int : { true, false }) {
                size_t n = 0;
                for (const auto& fmt : formats) {
                    if (fmt.bits() == bits && fmt.bytes() == bytes
                        && fmt.is_int() == is_int) {
                        if (n == 0) {
                            out << " ";
                        }
                        out << " " << fmt.to_string();
                        n++;
                    }
                }
                if (n != 0) {
                    out << "\n";
                }
            }
        }
    }
}

} // namespace signal_estimator
