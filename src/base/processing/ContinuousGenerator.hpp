// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "processing/IGenerator.hpp"

namespace signal_estimator {

// generate continuous sine wave
class ContinuousGenerator : public IGenerator {
public:
    ContinuousGenerator(const Config& config);

    ContinuousGenerator(const ContinuousGenerator&) = delete;
    ContinuousGenerator& operator=(const ContinuousGenerator&) = delete;

    void generate(Frame& frame) override;

private:
    const Config config_;
    size_t pos_ {};
};

} // namespace signal_estimator
