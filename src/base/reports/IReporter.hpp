// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

struct LatencyReport {
    double sw_hw { 0 };
    double hw { 0 };
    double hw_avg { 0 };
};

struct LossReport {
    double loss_rate { 0 };
    double loss_rate_avg { 0 };
    double loss_ratio { 0 };
};

struct IOJitterReport {
    double swdev_avg { 0 };
    double swdev_per { 0 };
    double hwdev_avg { 0 };
    double hwdev_per { 0 };
};

struct IODelayReport {
    double sw_avg { 0 };
    double hw_avg { 0 };
};

class IReporter {
public:
    virtual ~IReporter() = default;

    virtual void report(const LatencyReport& rep) = 0;
    virtual void report(const LossReport& rep) = 0;
    virtual void report(const IOJitterReport& rep) = 0;
    virtual void report(const IODelayReport& rep) = 0;
};

} // namespace signal_estimator
