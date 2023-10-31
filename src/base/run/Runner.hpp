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
#include "reports/JsonReporter.hpp"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

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
    void input_loop_(size_t dev_index);

    Config config_;

    std::unique_ptr<IDeviceWriter> output_writer_;
    std::vector<std::unique_ptr<IDeviceReader>> input_readers_;

    std::unique_ptr<FramePool> frame_pool_;

    std::unique_ptr<JsonPrinter> json_printer_;
    std::vector<std::unique_ptr<IReporter>> reporters_;

    std::unique_ptr<IGenerator> generator_;
    std::vector<std::unique_ptr<IEstimator>> estimators_;

    std::shared_ptr<IDumper> dumper_;

    std::thread output_thread_;
    std::vector<std::thread> input_threads_;

    std::atomic_bool stop_ { false };
    std::atomic_bool fail_ { false };
};

} // namespace signal_estimator
