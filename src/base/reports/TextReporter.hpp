// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"
#include "reports/IReporter.hpp"
#include "reports/TextPrinter.hpp"

#include <string>

namespace signal_estimator {

class TextReporter final : public IReporter {
public:
    TextReporter(const Config& config, const DevInfo& dev_info, TextPrinter& printer);

    void report(const LatencyReport& rep) override;
    void report(const LossReport& rep) override;
    void report(const IOJitterReport& rep) override;
    void report(const IODelayReport& rep) override;

private:
    Config config_;
    bool sign_ {};
    std::string suffix_;
    TextPrinter& printer_;
};

} // namespace signal_estimator
