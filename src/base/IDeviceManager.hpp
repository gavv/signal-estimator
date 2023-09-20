// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <string>
#include <string_view>
#include <vector>

class IDeviceManager {
public:
    virtual ~IDeviceManager() = default;

    virtual std::vector<std::string> get_output_devices() = 0;
    virtual std::vector<std::string> get_input_devices() = 0;
    virtual std::string format_device_name(std::string_view buffer) = 0;
};
