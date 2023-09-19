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
