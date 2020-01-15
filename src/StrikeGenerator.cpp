/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "StrikeGenerator.hpp"

#include <cstring>
#include <cmath>

namespace signal_estimator {

StrikeGenerator::StrikeGenerator(const Config& config)
    : config_(config)
    , strike_period_(size_t(config.sample_rate * config.strike_period))
    , strike_length_(size_t(config.sample_rate * config.strike_length))
    , warmup_countdown_(config.n_periods) {
}

void StrikeGenerator::generate(Frame& frame) {
    auto frame_data = frame.data();
    auto frame_size = frame.size();

    memset(frame_data, 0, frame_size * sizeof(frame_data[0]));

    if (warmup_countdown_ != 0) {
        warmup_countdown_--;
        return;
    }

    while (frame_size != 0) {
        if (strike_countdown_ != 0) {
            const size_t n_samples = frame_size / config_.n_channels;

            if (strike_countdown_ >= n_samples) {
                strike_countdown_ -= n_samples;
                return;
            }

            if (strike_countdown_ < n_samples) {
                frame_data += strike_countdown_ * config_.n_channels;
                frame_size -= strike_countdown_ * config_.n_channels;
                strike_countdown_ = 0;
            }
        }

        while (frame_size != 0) {
            if (++strike_pos_ == strike_length_) {
                strike_pos_ = 0;
                strike_countdown_ = strike_period_;
                break;
            }

            for (size_t cn = 0; cn < config_.n_channels; cn++) {
                *frame_data = sample_t(MaxSample * config_.volume
                    * std::sin(2 * M_PI / config_.sample_rate * 880 * strike_pos_));
                frame_data++;
                frame_size--;
            }
        }
    }
}

} // namespace signal_estimator
