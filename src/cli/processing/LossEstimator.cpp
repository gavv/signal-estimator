// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/LossEstimator.hpp"
#include "core/Pool.hpp"
#include "io/IFormatter.hpp"

#include <algorithm>
#include <memory>

namespace signal_estimator {

LossEstimator::LossEstimator(const Config& config, IFormatter& formatter)
    : config_(config)
    , signal_runmax_(config.signal_detection_window)
    , gradient_runmax_(config.glitch_detection_window)
    , gradient_schmitt_(config.glitch_detection_threshold)
    , sma_(config.sma_window)
    , format_(formatter) {
}

LossEstimator::~LossEstimator() {
}

void LossEstimator::add_output(std::shared_ptr<Frame> frame) {
    // noop
    (void)frame;
}

void LossEstimator::add_input(std::shared_ptr<Frame> frame) {
    if (!frame.get())
        return;
    auto frame_data = frame->data();
    auto frame_size = frame->size();

    for (size_t n = 0; n < frame_size; n++) {
        auto s = double(frame_data[n]);

        if (signal_runmax_.add(std::abs(s))
            >= MaxSample * config_.signal_detection_threshold) {
            signal_++;
            leading_zeros_ = false;
        } else {
            if (!leading_zeros_) {
                no_signal_++;
            }
        }

        s = gradient_.add(s);
        s = std::abs(s);
        s = gradient_runmax_.add(s);

        if (gradient_schmitt_.add(s)) {
            losses_++;
        }
    }

    report_losses_();
}

void LossEstimator::report_losses_() {
    const auto elapsed = limiter_.allow();

    if (elapsed > 0) {
        const double loss_rate = losses_ / elapsed;
        const double avg_loss_rate = sma_(loss_rate);

        const double loss_ratio = double(no_signal_) / (signal_ + no_signal_) * 100.0;

        format_.report_losses(
            loss_rate, (int)config_.sma_window, avg_loss_rate, loss_ratio);

        losses_ = 0;
        signal_ = no_signal_ = 0;
    }
}

} // namespace signal_estimator
