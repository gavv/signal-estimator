// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "run/Runner.hpp"
#include "core/Log.hpp"
#include "core/Realtime.hpp"
#include "dumps/AsyncDumper.hpp"
#include "dumps/CsvDumper.hpp"
#include "io/AlsaReader.hpp"
#include "io/AlsaWriter.hpp"
#include "processing/ContinuousGenerator.hpp"
#include "processing/CorrelationLatencyEstimator.hpp"
#include "processing/IODelayEstimator.hpp"
#include "processing/IOJitterEstimator.hpp"
#include "processing/Impulse.hpp"
#include "processing/ImpulseGenerator.hpp"
#include "processing/LossEstimator.hpp"
#include "processing/StepsGenerator.hpp"
#include "processing/StepsLatencyEstimator.hpp"
#include "reports/JsonReporter.hpp"
#include "reports/TextReporter.hpp"

#include <algorithm>

namespace signal_estimator {

Runner::Runner(const Config& config)
    : config_(config) {
}

Runner::~Runner() {
    stop();
    wait();
}

bool Runner::failed() const {
    return fail_;
}

bool Runner::start() {
    if (!config_.output_dev.empty()) {
        auto alsa_writer = std::make_unique<AlsaWriter>();
        if (!alsa_writer->open(config_, config_.output_dev)) {
            fail_ = true;
            return false;
        }
        config_.output_info = alsa_writer->info();
        output_writer_ = std::move(alsa_writer);
    }

    for (const auto& input_dev : config_.input_devs) {
        auto alsa_reader = std::make_unique<AlsaReader>();
        if (!alsa_reader->open(config_, input_dev)) {
            fail_ = true;
            return false;
        }
        config_.input_info.emplace_back(alsa_reader->info());
        input_readers_.emplace_back(std::move(alsa_reader));
    }

    config_.frame_size = config_.output_info.period_size;
    for (const auto& input_info : config_.input_info) {
        config_.frame_size = std::max(config_.frame_size, (size_t)input_info.period_size);
    }

    config_.show_device_names
        = config_.diff_inputs ? input_readers_.size() > 2 : input_readers_.size() > 1;

    if (!config_.dump_file.empty()) {
        auto csv_dumper = std::make_shared<CsvDumper>(config_);
        if (!csv_dumper->open(config_.dump_file)) {
            fail_ = true;
            return false;
        }
        dumper_ = std::move(csv_dumper);
    }

    if (dumper_) {
        dumper_ = std::make_shared<AsyncDumper>(std::move(dumper_));
    }

    se_log_info("starting measurement");

    frame_pool_ = std::make_unique<FramePool>(config_);

    size_t num_estimators = 0;
    if (input_readers_.size() != 0) {
        if (config_.diff_inputs) {
            // measurement for each input device except first
            num_estimators = input_readers_.size() - 1;
        } else {
            // measurement for each input device
            num_estimators = input_readers_.size();
        }
    } else if (output_writer_) {
        // measurements for one output device
        num_estimators = 1;
    }

    switch (config_.report_format) {
    case Format::Text:
        text_printer_ = std::make_unique<TextPrinter>(console_sink_);
        break;

    case Format::Json:
        json_printer_ = std::make_unique<JsonPrinter>(console_sink_);
        break;
    }

    for (size_t n = 0; n < num_estimators; n++) {
        DevInfo dev_info;
        if (input_readers_.size() != 0) {
            if (config_.diff_inputs) {
                // measurement for each input device except first
                dev_info = config_.input_info[n + 1];
            } else {
                // measurement for each input device
                dev_info = config_.input_info[n];
            }
        } else {
            dev_info = config_.output_info;
        }

        switch (config_.report_format) {
        case Format::Text:
            reporters_.emplace_back(std::make_unique<TextReporter>(config_, dev_info, *text_printer_));
            break;

        case Format::Json:
            reporters_.emplace_back(
                std::make_unique<JsonReporter>(config_, dev_info, *json_printer_));
            break;
        }

        switch (config_.mode) {
        case Mode::LatencyCorr:
            estimators_.emplace_back(
                std::make_unique<CorrelationLatencyEstimator>(config_, *reporters_[n]));
            break;

        case Mode::LatencyStep:
            estimators_.emplace_back(
                std::make_unique<StepsLatencyEstimator>(config_, *reporters_[n]));
            break;

        case Mode::Losses:
            estimators_.emplace_back(
                std::make_unique<LossEstimator>(config_, *reporters_[n]));
            break;

        case Mode::IOJitter:
            estimators_.emplace_back(
                std::make_unique<IOJitterEstimator>(config_, dev_info, *reporters_[n]));
            break;

        case Mode::IODelay:
            estimators_.emplace_back(
                std::make_unique<IODelayEstimator>(config_, dev_info, *reporters_[n]));
            break;
        }
    }

    if (output_writer_) {
        switch (config_.mode) {
        case Mode::LatencyCorr:
            generator_ = std::make_unique<ImpulseGenerator>(config_, impulse);
            break;

        case Mode::LatencyStep:
            generator_ = std::make_unique<StepsGenerator>(config_);
            break;

        case Mode::Losses:
            generator_ = std::make_unique<ContinuousGenerator>(config_);
            break;

        case Mode::IOJitter:
            generator_ = std::make_unique<ContinuousGenerator>(config_);
            break;

        case Mode::IODelay:
            generator_ = std::make_unique<ContinuousGenerator>(config_);
            break;
        }
    }

    if (output_writer_) {
        output_thread_ = std::thread(&Runner::output_loop_, this);
    }

    for (size_t n = 0; n < input_readers_.size(); n++) {
        input_threads_.emplace_back(std::thread(&Runner::input_loop_, this, n));
    }

    return true;
}

void Runner::stop() {
    stop_ = true;
}

void Runner::wait() {
    if (output_thread_.joinable()) {
        output_thread_.join();
    }

    for (auto& input_thread : input_threads_) {
        if (input_thread.joinable()) {
            input_thread.join();
        }
    }
}

void Runner::output_loop_() {
    if (!config_.no_realtime) {
        make_realtime();
    }

    uint64_t current_samples = 0, total_samples = config_.total_samples(),
             warmup_samples = config_.warmup_samples();

    while (current_samples < total_samples || total_samples == 0) {
        if (stop_ || fail_) {
            break;
        }

        auto frame = frame_pool_->allocate(Dir::Output, 0);

        if (generator_) {
            generator_->generate(*frame);
        }

        if (!output_writer_->write(*frame)) {
            se_log_error("got error from output device, exiting");
            fail_ = true;
            break;
        }

        current_samples += frame->size();

        if (current_samples < warmup_samples) {
            continue;
        }

        if (!config_.diff_inputs) {
            for (auto& estimator : estimators_) {
                estimator->add_output(frame);
            }
        }

        if (dumper_) {
            dumper_->write(frame);
        }
    }
}

void Runner::input_loop_(size_t dev_index) {
    if (!config_.no_realtime) {
        make_realtime();
    }

    uint64_t current_samples = 0, total_samples = config_.total_samples(),
             warmup_samples = config_.warmup_samples();

    while (current_samples < total_samples || total_samples == 0) {
        if (stop_ || fail_) {
            break;
        }

        auto frame = frame_pool_->allocate(Dir::Input, dev_index);

        if (!input_readers_[dev_index]->read(*frame)) {
            se_log_error("got error from input device, exiting");
            fail_ = true;
            break;
        }

        current_samples += frame->size();

        if (current_samples < warmup_samples) {
            continue;
        }

        if (!config_.diff_inputs) {
            estimators_[dev_index]->add_input(frame);
        } else {
            if (dev_index == 0) {
                for (auto& estimator : estimators_) {
                    estimator->add_output(frame);
                }
            } else {
                estimators_[dev_index - 1]->add_input(frame);
            }
        }

        if (dumper_) {
            dumper_->write(frame);
        }
    }
}

} // namespace signal_estimator
