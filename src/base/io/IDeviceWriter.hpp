// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

namespace signal_estimator {

class IDeviceWriter {
public:
    virtual ~IDeviceWriter() = default;

    virtual bool write(Frame& frame) = 0;
};

} // namespace signal_estimator
