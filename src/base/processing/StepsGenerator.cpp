// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/StepsGenerator.hpp"

#include <cmath>
#include <cstring>

namespace signal_estimator {

StepsGenerator::StepsGenerator(const Config& config)
    : config_(config)
    , step_period_(size_t(config.sample_rate * config.step_interval))
    , step_length_(size_t(config.sample_rate * config.step_length))
    , warmup_countdown_(config.output_info.period_count) {
}

void StepsGenerator::generate(Frame& frame) {
    std::fill_n(frame.data(), frame.size(), 0);

    if (warmup_countdown_ != 0) {
        warmup_countdown_--;
        return;
    }

    size_t frame_size = frame.size();
    size_t frame_pos = 0;

    while (frame_size != 0) {
        if (step_countdown_ != 0) {
            const size_t n_samples = frame_size / config_.channel_count;

            if (step_countdown_ >= n_samples) {
                step_countdown_ -= n_samples;
                return;
            }

            if (step_countdown_ < n_samples) {
                frame_pos += step_countdown_ * config_.channel_count;
                frame_size -= step_countdown_ * config_.channel_count;
                step_countdown_ = 0;
            }
        }

        while (frame_size != 0) {
            if (++step_pos_ == step_length_) {
                step_pos_ = 0;
                step_countdown_ = step_period_;
                break;
            }

            for (size_t cn = 0; cn < config_.channel_count; cn++) {
                frame[frame_pos] = saturated_cast(MaxSample * config_.gain
                    * std::sin(2 * (float)M_PI / config_.sample_rate * 880 * step_pos_));
                frame_pos++;
                frame_size--;
            }
        }
    }
}

} // namespace signal_estimator
