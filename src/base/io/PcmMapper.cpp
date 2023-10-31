// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/PcmMapper.hpp"
#include "io/PcmFuncs.hpp"

#include <cassert>
#include <cstring>

namespace signal_estimator {

PcmMapper::PcmMapper(
    const PcmFormat& in_fmt, size_t in_chans, const PcmFormat& out_fmt, size_t out_chans)
    : in_fmt_(in_fmt)
    , out_fmt_(out_fmt)
    , in_chans_(in_chans)
    , out_chans_(out_chans)
    , in_sample_sz_(pcm_sample_bytes(in_fmt.encoding))
    , out_sample_sz_(pcm_sample_bytes(out_fmt.encoding))
    , map_func_(pcm_mapper_func(
          in_fmt_.encoding, out_fmt_.encoding, in_fmt_.endian, out_fmt_.endian)) {
    assert(in_fmt == PcmFormat() || out_fmt == PcmFormat());
    assert(map_func_);
}

size_t PcmMapper::input_bytes(size_t samples_per_chan) const {
    return samples_per_chan * in_chans_ * in_sample_sz_;
}

size_t PcmMapper::output_bytes(size_t samples_per_chan) const {
    return samples_per_chan * out_chans_ * out_sample_sz_;
}

void PcmMapper::map(
    const uint8_t* in_samples, uint8_t* out_samples, size_t samples_per_chan) {
    assert(in_samples);
    assert(out_samples);

    for (size_t ns = 0; ns < samples_per_chan; ns++) {
        size_t in_pos = 0, out_pos = 0;

        while (in_pos < in_chans_ && out_pos < out_chans_) {
            map_func_(in_samples, out_samples);

            in_pos++;
            in_samples += in_sample_sz_;

            out_pos++;
            out_samples += out_sample_sz_;
        }

        while (out_pos < out_chans_) {
            map_func_(in_samples - in_sample_sz_, out_samples);

            out_pos++;
            out_samples += out_sample_sz_;
        }

        while (in_pos < in_chans_) {
            in_pos++;
            in_samples += in_sample_sz_;
        }
    }
}

} // namespace signal_estimator
