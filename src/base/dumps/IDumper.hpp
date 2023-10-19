// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

namespace signal_estimator {

class IDumper {
public:
    virtual ~IDumper() = default;

    virtual void write(FramePtr frame) = 0;
};

} // namespace signal_estimator
