// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/AsyncDumper.hpp"

namespace signal_estimator {

AsyncDumper::AsyncDumper(std::unique_ptr<IDumper> dumper)
    : dumper_(std::move(dumper))
    , thread_(&AsyncDumper::run_, this) {
}

AsyncDumper::~AsyncDumper() {
    queue_.push(nullptr);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void AsyncDumper::write(std::shared_ptr<Frame> frame) {
    queue_.push(frame);
}

void AsyncDumper::run_() {
    while (auto frame = queue_.wait_pop()) {
        dumper_->write(frame);
    }
}

} // namespace signal_estimator
