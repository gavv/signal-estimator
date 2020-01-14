/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <alsa/asoundlib.h>

namespace signal_estimator {

enum class Dir {
    Playback,
    Recording,
};

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
    unsigned int latency_us { 8000 };

    // number of samples per period
    size_t period_size { 0 };

    // interval between strikes in seconds
    float strike_period { 1 };

    // strike length in seconds
    float strike_length { 0.1 };

    // test duration
    float duration { 10 };

    // beep detection threshold
    float threshold { 0.4 };

    // latency SMA window
    size_t sma_window { 5 };

    // running maximum window for striek detection
    size_t runmax_window { 96 };

    // file dumper frame
    size_t dump_frame { 64 };

    // get test duration in samples
    size_t total_samples() const {
        return size_t(sample_rate * duration) * n_channels;
    }
};

} // namespace signal_estimator
