// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/FrameQueue.hpp"
#include "dumps/IDumper.hpp"

#include <memory>
#include <thread>

namespace signal_estimator {

// wraps another dumper and invokes it in background thread
class AsyncDumper : public IDumper {
public:
    AsyncDumper(std::shared_ptr<IDumper> dumper);
    ~AsyncDumper();

    AsyncDumper(const AsyncDumper&) = delete;
    AsyncDumper& operator=(const AsyncDumper&) = delete;

    void write(FramePtr frame) override;

private:
    void run_();

    std::shared_ptr<IDumper> dumper_;
    FrameQueue queue_;
    std::thread thread_;
};

} // namespace signal_estimator
