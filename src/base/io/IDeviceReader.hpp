// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

namespace signal_estimator {

class IDeviceReader {
public:
    virtual ~IDeviceReader() = default;

    virtual bool read(Frame& frame) = 0;
};

} // namespace signal_estimator
