// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/LossEstimator.hpp"
#include "reports/IReporter.hpp"

#include <algorithm>
#include <memory>

namespace signal_estimator {

LossEstimator::LossEstimator(const Config& config, IReporter& reporter)
    : config_(config)
    , signal_runmax_(config.signal_detection_window)
    , gradient_runmax_(config.glitch_detection_window)
    , gradient_schmitt_(config.glitch_detection_threshold)
    , sma_(config.report_sma_window)
    , reporter_(reporter) {
}

LossEstimator::~LossEstimator() {
}

void LossEstimator::add_output(FramePtr frame) {
    // noop
    (void)frame;
}

void LossEstimator::add_input(FramePtr frame) {
    if (!frame) {
        return;
    }

    process_frame_(*frame);

    report_losses_();
}

void LossEstimator::process_frame_(const Frame& frame) {
    for (size_t n = 0; n < frame.size(); n++) {
        auto s = (float)frame[n];

        if (signal_runmax_(std::abs(s))
            >= MaxSample * config_.signal_detection_threshold) {
            signal_++;
            leading_zeros_ = false;
        } else {
            if (!leading_zeros_) {
                no_signal_++;
            }
        }

        s = gradient_(s);
        s = std::abs(s);
        s = gradient_runmax_(s);

        if (gradient_schmitt_(s)) {
            losses_++;
        }
    }
}

void LossEstimator::report_losses_() {
    const auto elapsed = limiter_.allow();

    if (elapsed > 0) {
        const double loss_rate = (double)losses_ / elapsed;
        const double avg_loss_rate = sma_(loss_rate);

        const double loss_ratio = double(no_signal_) / (signal_ + no_signal_) * 100.0;

        reporter_.report_losses(
            loss_rate, avg_loss_rate, loss_ratio, (int)config_.report_sma_window);

        losses_ = 0;
        signal_ = no_signal_ = 0;
    }
}

} // namespace signal_estimator
