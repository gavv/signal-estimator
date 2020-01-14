/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "SignalGenerator.hpp"

#include <cstring>
#include <cmath>

namespace signal_estimator {

SignalGenerator::SignalGenerator(const Config& config)
    : config_(config)
    , strike_period_(size_t(config.sample_rate * config.strike_period))
    , strike_length_(size_t(config.sample_rate * config.strike_length))
    , warmup_countdown_(config.n_periods) {
}

void SignalGenerator::generate(int16_t* buf, size_t bufsz) {
    memset(buf, 0, bufsz * sizeof(buf[0]));

    if (warmup_countdown_ != 0) {
        warmup_countdown_--;
        return;
    }

    while (bufsz != 0) {
        if (strike_countdown_ != 0) {
            const size_t n_frames = bufsz / config_.n_channels;

            if (strike_countdown_ >= n_frames) {
                strike_countdown_ -= n_frames;
                return;
            }

            if (strike_countdown_ < n_frames) {
                buf += strike_countdown_ * config_.n_channels;
                bufsz -= strike_countdown_ * config_.n_channels;
                strike_countdown_ = 0;
            }
        }

        while (bufsz != 0) {
            if (++strike_pos_ == strike_length_) {
                strike_pos_ = 0;
                strike_countdown_ = strike_period_;
                break;
            }

            for (size_t cn = 0; cn < config_.n_channels; cn++) {
                *buf = int16_t(
                    32767 * std::sin(2 * M_PI / config_.sample_rate * 880 * strike_pos_));
                buf++;
                bufsz--;
            }
        }
    }
}

} // namespace signal_estimator
