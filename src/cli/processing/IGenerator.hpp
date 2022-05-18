// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

// generate output signal
class IGenerator {
public:
    virtual ~IGenerator() = default;

    virtual void generate(Frame& frame) = 0;
};

} // namespace signal_estimator
