// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Dir.hpp"

#include <string>

namespace signal_estimator {

struct DevInfo {
    // input or output device
    Dir dir;

    // short name of device for reports and dumps
    std::string short_name;

    // number of periods in hardware ring buffer
    // may differ from Config::requested_period_count, for each
    // device we use what it supports
    unsigned int period_count { 0 };

    // number of samples (for all channels) in period
    // based on Config::requested_XXX_latency_us and
    // Config::requested_XXX_period_count
    unsigned int period_size { 0 };

    // number of channels
    // may differ from Config::channel_count, for each
    // device we use what it supports
    unsigned int channel_count { 0 };
};

} // namespace signal_estimator
