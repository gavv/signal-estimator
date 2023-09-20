// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "AlsaDeviceManager.hpp"

#include <cstdio>
#include <cstring>
#include <memory>

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
    const char* c = (std::strstr(buffer.data(), "card ") + 5); // get card number
    result += *c; // hw:X
    c = (std::strstr(buffer.data(), " device ") + 8); // get device
    result += ","; // hw:X,
    result += *c; // hw:X,Y
    return result;
}
