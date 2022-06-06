// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/StepsGenerator.hpp"

#include <cmath>
#include <cstring>

namespace signal_estimator {

StepsGenerator::StepsGenerator(const Config& config)
    : config_(config)
    , step_period_(size_t(config.sample_rate * config.step_period))
    , step_length_(size_t(config.sample_rate * config.step_length))
    , warmup_countdown_(config.io_num_periods) {
}

void StepsGenerator::generate(Frame& frame) {
    auto frame_data = frame.data();
    auto frame_size = frame.size();

    memset(frame_data, 0, frame_size * sizeof(frame_data[0]));

    if (warmup_countdown_ != 0) {
        warmup_countdown_--;
        return;
    }

    while (frame_size != 0) {
        if (step_countdown_ != 0) {
            const size_t n_samples = frame_size / config_.n_channels;

            if (step_countdown_ >= n_samples) {
                step_countdown_ -= n_samples;
                return;
            }

            if (step_countdown_ < n_samples) {
                frame_data += step_countdown_ * config_.n_channels;
                frame_size -= step_countdown_ * config_.n_channels;
                step_countdown_ = 0;
            }
        }

        while (frame_size != 0) {
            if (++step_pos_ == step_length_) {
                step_pos_ = 0;
                step_countdown_ = step_period_;
                break;
            }

            for (size_t cn = 0; cn < config_.n_channels; cn++) {
                *frame_data = sample_t(MaxSample * config_.volume
                    * std::sin(2 * M_PI / config_.sample_rate * 880 * step_pos_));
                frame_data++;
                frame_size--;
            }
        }
    }
}

} // namespace signal_estimator
