// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"
#include "reports/IReporter.hpp"
#include "reports/JsonPrinter.hpp"

#include <string>

namespace signal_estimator {

class JsonReporter final : public IReporter {
public:
    JsonReporter(const Config& config, const DevInfo& dev_info, JsonPrinter& printer);

    void report(const LatencyReport& rep) override;
    void report(const LossReport& rep) override;
    void report(const IOJitterReport& rep) override;
    void report(const IODelayReport& rep) override;

private:
    const std::string dev_name_;
    JsonPrinter& printer_;
};

} // namespace signal_estimator
