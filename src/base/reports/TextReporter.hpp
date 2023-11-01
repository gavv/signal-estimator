// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "reports/IReporter.hpp"

#include <string>

namespace signal_estimator {

class TextReporter : public IReporter {
public:
    TextReporter(const Config& config, const DevInfo& dev_info);

    TextReporter(const TextReporter&) = delete;
    TextReporter& operator=(const TextReporter&) = delete;

    void report(const LatencyReport& rep) override;
    void report(const LossReport& rep) override;
    void report(const IOJitterReport& rep) override;
    void report(const IODelayReport& rep) override;

private:
    Config config_;
    bool sign_ {};
    std::string suffix_;
};

} // namespace signal_estimator
