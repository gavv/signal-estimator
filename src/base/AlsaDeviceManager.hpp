// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "IDeviceManager.hpp"

#include <string>
#include <vector>

struct AlsaDeviceManager : IDeviceManager {
    std::vector<std::string> get_output_devices() override;
    std::vector<std::string> get_input_devices() override;
};
