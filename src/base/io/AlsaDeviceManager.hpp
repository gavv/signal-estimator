// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "io/IDeviceManager.hpp"

#include <string>
#include <vector>

namespace signal_estimator {

class AlsaDeviceManager : public IDeviceManager {
public:
    std::vector<std::string> get_output_devices() override;
    std::vector<std::string> get_input_devices() override;
    std::string format_device_name(std::string_view buffer) override;
};

} // namespace signal_estimator
