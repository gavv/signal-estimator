/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class SignalGenerator {
public:
    SignalGenerator(const Config& config);

    SignalGenerator(const SignalGenerator&) = delete;
    SignalGenerator& operator=(const SignalGenerator&) = delete;

    void generate(int16_t* buf, size_t bufsz);

private:
    const Config config_;

    const size_t strike_period_;
    const size_t strike_length_;

    size_t warmup_countdown_ {};

    size_t strike_countdown_ {};
    size_t strike_pos_ {};
};

} // namespace signal_estimator
