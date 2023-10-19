// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaDeviceManager.hpp"

#include <cstdio>
#include <cstring>
#include <memory>

namespace signal_estimator {

std::vector<std::string> AlsaDeviceManager::get_output_devices() {
    char buffer[128];
    std::vector<std::string> strvec;
    // run pipe to get output devices from aplay
    std::unique_ptr<FILE, decltype(&pclose)> pipe1(popen("aplay -l", "r"), pclose);

    if (!pipe1) {
        return strvec;
    }

    // read everything from aplay into result
    while (fgets(buffer, sizeof(buffer), pipe1.get()) != nullptr) {
        // if line has both card and device in it
        if (std::strstr(buffer, "card") != nullptr
            && std::strstr(buffer, "device") != nullptr) {
            strvec.emplace_back(buffer);
        }
    }
    return strvec;
}

std::vector<std::string> AlsaDeviceManager::get_input_devices() {
    char buffer[128];
    std::vector<std::string> strvec;
    // run pipe to get output devices from arecord
    std::unique_ptr<FILE, decltype(&pclose)> pipe2(popen("arecord -l", "r"), pclose);
    if (!pipe2) {
        return strvec;
    }

    while (fgets(buffer, sizeof(buffer), pipe2.get()) != nullptr) {
        // if line has both card and device in it
        if (std::strstr(buffer, "card") != nullptr
            && std::strstr(buffer, "device") != nullptr) {
            strvec.emplace_back(buffer);
        }
    }
    return strvec;
}

std::string AlsaDeviceManager::format_device_name(std::string_view buffer) {
    if (buffer == "default" || buffer.empty()) {
        return std::string { buffer };
    }

    std::string result { "hw:" };

    const char* card_prefix = "card ";
    if (const char* str = std::strstr(buffer.data(), card_prefix)) {
        result += *(str + strlen(card_prefix));
    }

    const char* device_prefix = " device ";
    if (const char* str = std::strstr(buffer.data(), device_prefix)) {
        result += ',';
        result += *(str + strlen(device_prefix));
    }

    return result;
}

} // namespace signal_estimator
