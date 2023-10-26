// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "dumps/AsyncDumper.hpp"

namespace signal_estimator {

AsyncDumper::AsyncDumper(std::shared_ptr<IDumper> dumper)
    : dumper_(std::move(dumper))
    , thread_(&AsyncDumper::run_, this) {
}

AsyncDumper::~AsyncDumper() {
    queue_.push(nullptr);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void AsyncDumper::write(FramePtr frame) {
    queue_.push(std::move(frame));
}

void AsyncDumper::run_() {
    while (auto frame = queue_.wait_pop()) {
        dumper_->write(frame);
    }
}

} // namespace signal_estimator
