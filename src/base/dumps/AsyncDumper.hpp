// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Queue.hpp"
#include "dumps/IDumper.hpp"

#include <memory>
#include <thread>

namespace signal_estimator {

// wraps another dumper and invokes it in background thread
class AsyncDumper : public IDumper {
public:
    AsyncDumper(std::unique_ptr<IDumper> dumper);
    ~AsyncDumper();

    AsyncDumper(const AsyncDumper&) = delete;
    AsyncDumper& operator=(const AsyncDumper&) = delete;

    void write(std::shared_ptr<Frame> frame) override;

private:
    void run_();

    std::unique_ptr<IDumper> dumper_;
    Queue<std::shared_ptr<Frame>> queue_;
    std::thread thread_;
};

} // namespace signal_estimator
