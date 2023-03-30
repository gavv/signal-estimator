// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/ImpulseGenerator.hpp"
#include "core/Sample.hpp"

#include <cassert>
#include <cstring>

namespace signal_estimator {

void ImpulseGenerator::generate(Frame& frame) {
    const auto num_ch = config_.n_channels;

    const auto frame_data = frame.data();
    const auto frame_size = frame.size();

    size_t i_frame = 0;

    do {
        for (; i_frame < frame_size && counter_ < impulse_size_
             && counter_ < impulse_period_;
             ++counter_, i_frame += num_ch) {

            assert(i_frame % num_ch == 0);
            assert(i_frame <= frame_size - num_ch);

            for (size_t j = 0; j <= num_ch; ++j) {
                assert(i_frame + j < frame_size);

                frame_data[i_frame + j] = sample_t(impulse_data_[counter_] * MaxSample);
            }
        }

        if (i_frame < frame_size && counter_ < impulse_period_) {
            const size_t n_fill
                = std::min((frame_size - i_frame), (impulse_period_ - counter_) * num_ch);

            assert(n_fill % num_ch == 0);
            assert(i_frame + n_fill <= frame_size);

            memset(frame_data + i_frame, 0, n_fill * sizeof(sample_t));

            counter_ += n_fill / num_ch;
            i_frame += n_fill;
        } else if (counter_ >= impulse_period_) {
            counter_ = 0;
        }
    } while (i_frame < frame_size);
}

} // namespace signal_estimator
