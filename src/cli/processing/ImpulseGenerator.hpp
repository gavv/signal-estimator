// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "processing/IGenerator.hpp"

#include <vector>

namespace signal_estimator {

template <typename T> class ImpulseGenerator : public IGenerator {
public:
    ImpulseGenerator(const Config& config, const T& impulse)
        : config_(config)
        , impulse_(impulse)
        , impulse_period_(size_t(config.impulse_period * config.sample_rate))
        , counter_(0) {
    }

    ImpulseGenerator& operator=(const ImpulseGenerator&) = delete;

    void generate(Frame& frame) override {
        size_t i_frame = 0;
        do {
            for (; i_frame < frame.size() && counter_ < impulse_.size()
                 && counter_ < impulse_period_;
                 ++counter_, i_frame += config_.n_channels) {
                for (size_t j = 0; j <= config_.n_channels; ++j) {
                    *(frame.data() + i_frame + j) = sample_t(impulse_[counter_] * 32768.);
                }
            }
            if (i_frame < frame.size() && counter_ < impulse_period_) {
                size_t n_fill = std::min((frame.size() - i_frame),
                    (impulse_period_ - counter_) * config_.n_channels);
                memset(frame.data() + i_frame, 0, n_fill * sizeof(sample_t));
                counter_ += n_fill / config_.n_channels;
                i_frame += n_fill;
            } else if (counter_ >= impulse_period_) {
                counter_ = 0;
            }
        } while (i_frame < frame.size());
    }

private:
    const Config config_;

    const T& impulse_;
    const size_t impulse_period_;
    size_t counter_;
};

} // namespace signal_estimator
