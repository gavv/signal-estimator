// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/ImpulseGenerator.hpp"
#include "processing/Sample.hpp"

#include <cstring>

namespace signal_estimator {

void ImpulseGenerator::generate(Frame& frame) {
    size_t i_frame = 0;
    do {
        for (; i_frame < frame.size() && counter_ < impulse_size_
             && counter_ < impulse_period_;
             ++counter_, i_frame += config_.n_channels) {
            for (size_t j = 0; j <= config_.n_channels; ++j) {
                *(frame.data() + i_frame + j)
                    = sample_t(impulse_data_[counter_] * MaxSample);
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

} // namespace signal_estimator
