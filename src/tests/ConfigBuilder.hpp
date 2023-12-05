// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"

class ConfigBuilder {
public:
    signal_estimator::Config build() {
        return config_;
    }

    ConfigBuilder& set_diff_inputs(bool diff_inputs) {
        config_.diff_inputs = diff_inputs;
        return *this;
    }

    ConfigBuilder& set_show_device_names(bool show_device_names) {
        config_.show_device_names = show_device_names;
        return *this;
    }

private:
    signal_estimator::Config config_;
};