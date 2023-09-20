// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "IDeviceManager.hpp"

#include <string>
#include <vector>

class AlsaDeviceManager : public IDeviceManager {
public:
    std::vector<std::string> get_output_devices() override;
    std::vector<std::string> get_input_devices() override;
    std::string format_device_name(std::string_view buffer) override;
};
