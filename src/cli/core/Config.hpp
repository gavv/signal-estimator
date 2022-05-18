// Copyright (c) Signal Estimator authors
// Licensed under MIT

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

    // test signal volume
    float volume { 0.5f };

    // periods (bursts) per buffer
    unsigned int n_periods { 2 };

    // desired alsa latency in microseconds
    unsigned int latency_us { 8000 };

    // number of samples per period
    size_t period_size { 0 };

    // test duration
    float duration { 10 };

    // how long do not process signals in seconds
    float warmup_duration { 1 };

    // interval between strikes in seconds
    float strike_period { 1 };

    // strike length in seconds
    float strike_length { 0.1f };

    // running maximum window for strike detection
    size_t strike_detection_window { 96 };

    // strike detection threshold
    float strike_detection_threshold { 0.4f };

    // running maximum window for signal detection
    size_t signal_detection_window { 48 };

    // signal detection threshold
    float signal_detection_threshold { 0.15f };

    // glitch detection window
    size_t glitch_detection_window { 32 };

    // glitch detection threshold
    float glitch_detection_threshold { 0.05f };

    // latency SMA window
    size_t sma_window { 5 };

    // the impulse latency estimator peak detection window length in samples
    size_t impulse_peak_detection_width { 128 };

    // the peak-to-noise minimum ratio threshold
    float impulse_avg_2_peak_ration_threshold { 8.0 };

    // interval between impulses in seconds
    float impulse_period { 1 };

    // file dumper frame
    size_t dump_frame { 64 };

    // file dumper rounding
    size_t dump_rounding { 10 };

    // get test duration in samples
    size_t total_samples() const {
        return size_t(sample_rate * duration) * n_channels;
    }
};

} // namespace signal_estimator
