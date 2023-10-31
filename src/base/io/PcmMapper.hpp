// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "io/PcmFormat.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

// PCM format mapper
// Maps encoding, endian, and channel count
// Input or output format should be default format (or both)
class PcmMapper {
public:
    PcmMapper(const PcmFormat& in_fmt, size_t in_chans, const PcmFormat& out_fmt,
        size_t out_chans);

    PcmMapper(const PcmMapper&) = delete;
    PcmMapper& operator=(const PcmMapper&) = delete;

    size_t input_bytes(size_t samples_per_chan) const;
    size_t output_bytes(size_t samples_per_chan) const;

    void map(const uint8_t* in_samples, uint8_t* out_samples, size_t samples_per_chan);

private:
    const PcmFormat in_fmt_;
    const PcmFormat out_fmt_;

    const size_t in_chans_;
    const size_t out_chans_;

    const size_t in_sample_sz_;
    const size_t out_sample_sz_;

    void (*const map_func_)(const uint8_t* in_data, uint8_t* out_data);
};

} // namespace signal_estimator
