// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "reports/IReporter.hpp"
#include "reports/JsonPrinter.hpp"

#include <string>

namespace signal_estimator {

class JsonReporter : public IReporter {
public:
    JsonReporter(const Config& config, const DevInfo& dev_info, JsonPrinter& printer);

    JsonReporter(const JsonReporter&) = delete;
    JsonReporter& operator=(const JsonReporter&) = delete;

    void report(const LatencyReport& rep) override;
    void report(const LossReport& rep) override;
    void report(const IOJitterReport& rep) override;
    void report(const IODelayReport& rep) override;

private:
    JsonPrinter& printer_;
    const std::string dev_name_;
};

} // namespace signal_estimator
