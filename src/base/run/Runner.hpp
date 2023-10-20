// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/FramePool.hpp"
#include "dumps/IDumper.hpp"
#include "io/IDeviceReader.hpp"
#include "io/IDeviceWriter.hpp"
#include "processing/IEstimator.hpp"
#include "processing/IGenerator.hpp"
#include "reports/IReporter.hpp"

#include <atomic>
#include <memory>
#include <thread>

namespace signal_estimator {

class Runner {
public:
    Runner(const Config& config);
    ~Runner();

    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;

    bool failed() const;

    bool start();
    void stop();
    void wait();

private:
    void output_loop_();
    void input_loop_();

    Config config_;

    std::unique_ptr<IDeviceWriter> output_writer_;
    std::unique_ptr<IDeviceReader> input_reader_;

    std::unique_ptr<FramePool> frame_pool_;

    std::unique_ptr<IReporter> reporter_;

    std::unique_ptr<IGenerator> generator_;
    std::unique_ptr<IEstimator> estimator_;

    std::unique_ptr<IDumper> output_dumper_;
    std::unique_ptr<IDumper> input_dumper_;

    std::thread output_thread_;
    std::thread input_thread_;

    std::atomic_bool stop_ { false };
    std::atomic_bool fail_ { false };
};

} // namespace signal_estimator
