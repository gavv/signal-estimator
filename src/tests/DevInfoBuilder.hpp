// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/DevInfo.hpp"

#include <string>

class DevInfoBuilder {
public:
    signal_estimator::DevInfo build() {
        return dev_info_;
    }

    DevInfoBuilder& set_short_name(const std::string& short_name) {
        dev_info_.short_name = short_name;
        return *this;
    }

private:
    signal_estimator::DevInfo dev_info_;
};
