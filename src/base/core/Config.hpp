// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/DevInfo.hpp"
#include "core/Dir.hpp"
#include "core/Time.hpp"
#include "io/PcmFormat.hpp"

#include <cstdlib>
#include <string>
#include <vector>

namespace signal_estimator {

enum class Mode {
    LatencyCorr,
    LatencyStep,
    Losses,
    IOJitter,
};

enum class Format {
    Text,
    Json,
};

struct Config {
    // operation mode
    Mode mode { Mode::LatencyCorr };

    // report format
    Format report_format { Format::Text };

    // whether to show device names in reports and dumps
    bool show_device_names { false };

    // instead of output + input, pass two inputs to estimator
    bool diff_inputs { false };

    // disable using SCHED_RR policy
    bool no_realtime { false };

    // device names
    std::string output_dev;
    std::vector<std::string> input_devs;

    // dump file
    std::string dump_file;

    // total measurement duration
    float measurement_duration { 0 };

    // how long to ignore input after startup, in seconds
    float warmup_duration { 1 };

    // number of samples per second per channel
    unsigned int sample_rate { 48000 };

    // channel count for processing
    unsigned int channel_count { 2 };

    // probe signal gain
    float gain { 0.8f };

    // desired output latency, in microseconds
    unsigned int requested_output_latency_us { 8000 };

    // desired number of periods in output ring buffer(s)
    unsigned int requested_output_period_count { 2 };

    // desired format for output device
    PcmFormat requested_output_format {};

    // desired input latency, in microseconds
    unsigned int requested_input_latency_us { 8000 };

    // desired number of periods in input ring buffer(s)
    unsigned int requested_input_period_count { 2 };

    // desired format for input device
    PcmFormat requested_input_format {};

    // output device info
    DevInfo output_info;

    // input device(s) info
    std::vector<DevInfo> input_info;

    // sma window for latency reports, in seconds
    size_t report_sma_window { 5 };

    // dump compression ratio
    size_t dump_compression { 0 };

    // interval between strikes in seconds
    float step_interval { 1 };

    // strike length in seconds
    float step_length { 0.1f };

    // running maximum window for strike detection
    size_t step_detection_window { 96 };

    // strike detection threshold
    float step_detection_threshold { 0.4f };

    // the impulse latency estimator peak detection window length in samples
    size_t impulse_peak_detection_width { 128 };

    // the peak-to-noise minimum ratio threshold
    float impulse_avg_2_peak_ration_threshold { 4.0 };

    // interval between impulses in seconds
    float impulse_interval { 1 };

    // running maximum window for signal detection
    size_t signal_detection_window { 48 };

    // signal detection threshold
    float signal_detection_threshold { 0.15f };

    // glitch detection window
    size_t glitch_detection_window { 32 };

    // glitch detection threshold
    float glitch_detection_threshold { 0.05f };

    // running window for jitter calculation, number of periods
    size_t io_jitter_window { 250 };

    // percentile for jitter reports, from 0 to 100
    size_t io_jitter_percentile { 95 };

    // default number of samples for each frame
    size_t frame_size { 0 };

    // number of pre-allocated frames in frame pool
    size_t frame_pool_size { 128 };

    // get warmup duration in samples
    size_t warmup_samples() const {
        return size_t(sample_rate * warmup_duration) * channel_count;
    }

    // get test duration in samples
    size_t total_samples() const {
        return size_t(sample_rate * measurement_duration) * channel_count;
    }

    // convert number of samples (all channels) to number of nanoseconds
    nanoseconds_t samples_to_ns(size_t num_samples) const {
        return nanoseconds_t(num_samples) / channel_count * Second / sample_rate;
    }

    // convert number of samples (per channel) to number of nanoseconds
    nanoseconds_t frames_to_ns(size_t num_samples) const {
        return nanoseconds_t(num_samples) * Second / sample_rate;
    }
};

} // namespace signal_estimator
