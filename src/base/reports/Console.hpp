// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

namespace signal_estimator {

class Console {
public:
    virtual void write(const char* str);
    virtual void flush();
};

} // namespace signal_estimator
