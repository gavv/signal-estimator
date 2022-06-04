// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

#include <memory>

namespace signal_estimator {

class IDumper {
public:
    virtual ~IDumper() = default;

    virtual void write(std::shared_ptr<Frame> frame) = 0;
};

} // namespace signal_estimator
