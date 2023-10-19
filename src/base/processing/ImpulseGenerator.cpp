// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/ImpulseGenerator.hpp"
#include "core/Sample.hpp"

#include <cassert>
#include <cstring>

namespace signal_estimator {

void ImpulseGenerator::generate(Frame& frame) {
    const auto num_ch = config_.n_channels;

    size_t i_frame = 0;

    do {
        for (; i_frame < frame.size() && counter_ < impulse_size_
             && counter_ < impulse_period_;
             ++counter_, i_frame += num_ch) {

            assert(i_frame % num_ch == 0);
            assert(i_frame <= frame.size() - num_ch);

            for (size_t j = 0; j < num_ch; ++j) {
                frame[i_frame + j] = sample_t(impulse_data_[counter_] * MaxSample);
            }
        }

        if (i_frame < frame.size() && counter_ < impulse_period_) {
            const size_t n_fill = std::min(
                (frame.size() - i_frame), (impulse_period_ - counter_) * num_ch);

            assert(n_fill % num_ch == 0);
            assert(i_frame + n_fill <= frame.size());

            std::fill_n(frame.data() + i_frame, n_fill, 0);

            counter_ += n_fill / num_ch;
            i_frame += n_fill;
        } else if (counter_ >= impulse_period_) {
            counter_ = 0;
        }
    } while (i_frame < frame.size());
}

} // namespace signal_estimator
