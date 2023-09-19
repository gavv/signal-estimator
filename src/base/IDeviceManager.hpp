// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <string>
#include <vector>

struct IDeviceManager {
    virtual ~IDeviceManager() = default;

    virtual std::vector<std::string> get_output_devices() = 0;
    virtual std::vector<std::string> get_input_devices() = 0;
};
