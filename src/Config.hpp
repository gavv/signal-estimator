/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <alsa/asoundlib.h>

namespace signal_estimator {

struct Config {
    // interleaved samples (L R L R ...)
    snd_pcm_access_t access { SND_PCM_ACCESS_RW_INTERLEAVED };

    // 16-bit little-endian signed integers
    snd_pcm_format_t format { SND_PCM_FORMAT_S16_LE };

    // number of samples per second per channel
    unsigned int sample_rate { 48000 };

    // stereo (L and R)
    unsigned int n_channels { 2 };

    // periods (bursts) per buffer
    unsigned int n_periods { 2 };

    // desired alsa latency in microseconds
    unsigned int latency_us { 10000 };

    // interval between strikes in seconds
    float strike_period { 2 };

    // strike length in seconds
    float strike_length { 0.1 };

    // test duration
    float duration { 5 };

    // get test duration in samples
    size_t total_samples() const {
        return size_t(sample_rate * duration) * n_channels;
    }
};

} // namespace signal_estimator
