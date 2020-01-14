/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "ContinuousGenerator.hpp"

#include <cstring>
#include <cmath>

namespace signal_estimator {

ContinuousGenerator::ContinuousGenerator(const Config& config)
    : config_(config) {
}

void ContinuousGenerator::generate(int16_t* buf, size_t bufsz) {
    for (size_t sn = 0; sn < bufsz;) {
        for (size_t cn = 0; cn < config_.n_channels; cn++) {
            buf[sn++]
                = int16_t(20000 * std::sin(2 * M_PI / config_.sample_rate * 500 * pos_));
        }
        pos_++;
    }
}

} // namespace signal_estimator
