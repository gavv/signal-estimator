// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "processing/IGenerator.hpp"

namespace signal_estimator {

// generate periodic steps (beeps)
class StepsGenerator : public IGenerator {
public:
    StepsGenerator(const Config& config);

    StepsGenerator(const StepsGenerator&) = delete;
    StepsGenerator& operator=(const StepsGenerator&) = delete;

    void generate(Frame& frame) override;

private:
    const Config config_;

    const size_t step_period_;
    const size_t step_length_;

    size_t warmup_countdown_ {};

    size_t step_countdown_ {};
    size_t step_pos_ {};
};

} // namespace signal_estimator
