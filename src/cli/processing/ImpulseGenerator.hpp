// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "processing/IGenerator.hpp"

#include <cstdlib>

namespace signal_estimator {

class ImpulseGenerator : public IGenerator {
public:
    template <class T>
    ImpulseGenerator(const Config& config, const T& impulse)
        : config_(config)
        , impulse_data_(impulse.data())
        , impulse_size_(impulse.size())
        , impulse_period_(size_t(config.impulse_period * config.sample_rate)) {
    }

    ImpulseGenerator(const ImpulseGenerator&) = delete;
    ImpulseGenerator& operator=(const ImpulseGenerator&) = delete;

    void generate(Frame& frame) override;

private:
    const Config config_;

    const float* impulse_data_;
    size_t impulse_size_;
    const size_t impulse_period_;

    size_t counter_ { 0 };
};

} // namespace signal_estimator
