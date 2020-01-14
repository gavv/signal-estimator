/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "LossEstimator.hpp"
#include "Log.hpp"

#include <algorithm>

namespace signal_estimator {

LossEstimator::LossEstimator(const Config& config)
    : config_(config)
    , runmax_(config.glitch_window)
    , schmitt_(config.glitch_threshold)
    , sma_(config.sma_window) {
}

void LossEstimator::add_output(Frame&) {
    // noop
}

void LossEstimator::add_input(Frame& frame) {
    auto frame_data = frame.data();
    auto frame_size = frame.size();

    for (size_t n = 0; n < frame_size; n++) {
        auto s = double(frame_data[n]);

        s = gradient_.add(s);
        s = std::abs(s);
        s = runmax_.add(s);

        if (schmitt_.add(s)) {
            losses_++;
        }
    }

    report_losses_();
}

void LossEstimator::report_losses_() {
    const auto elapsed = limiter_.allow();

    if (elapsed > 0) {
        const double losses_per_sec = losses_ / elapsed;
        const double avg_losses_per_sec = sma_.add(losses_per_sec);

        se_log_info("losses:  cur %5.1f/sec  avg%d %5.1f/sec", losses_per_sec,
                    (int)config_.sma_window, avg_losses_per_sec);

        losses_ = 0;
    }
}

} // namespace signal_estimator
