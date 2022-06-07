// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/FramePool.hpp"
#include "core/Log.hpp"
#include "core/Realtime.hpp"
#include "fmt/AsyncDumper.hpp"
#include "fmt/CsvDumper.hpp"
#include "fmt/IDumper.hpp"
#include "fmt/IFormatter.hpp"
#include "fmt/JsonFormatter.hpp"
#include "fmt/TextFormatter.hpp"
#include "processing/ContinuousGenerator.hpp"
#include "processing/CorrelationLatencyEstimator.hpp"
#include "processing/Impulse.hpp"
#include "processing/ImpulseGenerator.hpp"
#include "processing/LossEstimator.hpp"
#include "processing/StepsGenerator.hpp"
#include "processing/StepsLatencyEstimator.hpp"
#include "sndio/AlsaReader.hpp"
#include "sndio/AlsaWriter.hpp"

#include <iostream>
#include <memory>
#include <memory>
#include <thread>
#include <vector>

#include <cxxopts.hpp>

using namespace signal_estimator;

namespace {

void output_loop(const Config* config, FramePool* frame_pool, IGenerator* generator,
    IEstimator* estimator, AlsaWriter* writer, IDumper* dumper) {
    set_realtime();

    size_t n = 0;

    for (; config->io_num_periods > n; n++) {
        auto frame = frame_pool->allocate();

        if (!writer->write(*frame)) {
            exit(1);
        }
    }

    for (; n < config->total_periods(); n++) {
        auto frame = frame_pool->allocate();

        generator->generate(*frame);

        if (!writer->write(*frame)) {
            exit(1);
        }

        if (n < config->warmup_periods()) {
            continue;
        }

        if (estimator) {
            estimator->add_output(frame);
        }

        if (dumper) {
            dumper->write(frame);
        }
    }

    // Kill the estimator's thread.
    if (estimator) {
        estimator->add_output(nullptr);
    }
}

void input_loop( const Config* config, FramePool* frame_pool, IEstimator* estimator,
    AlsaReader* reader, IDumper* dumper) {
    set_realtime();
    for (size_t n = 0; n < config->total_periods(); n++) {
        auto frame = frame_pool->allocate();

        if (!reader->read(*frame)) {
            exit(1);
        }

        if (n < config->warmup_periods()) {
            continue;
        }

        if (estimator) {
            estimator->add_input(frame);
        }

        if (dumper) {
            dumper->write(frame);
        }
    }

    // Kill the estimator's thread.
    if (estimator) {
        estimator->add_input(nullptr);
    }
}

} // namespace

int main(int argc, char** argv) {
    Config config;

    cxxopts::Options opts(
        "signal-estimator", "Measure characteristics of a looped back signal");

    opts.add_options("General")
        ("h,help", "Print help message and exit")
        ("m,mode", "Mode: noop|latency_corr|latency_step|losses",
         cxxopts::value<std::string>()->default_value("latency_corr"))
        ("o,output", "Output device name, required",
         cxxopts::value<std::string>())
        ("i,input", "Input device name, required",
         cxxopts::value<std::string>())
        ("d,duration", "Measurement duration, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.measurement_duration)))
        ("w,warmup", "Warmup duration, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.warmup_duration)))
        ;

    opts.add_options("I/O")
        ("r,rate", "Sample rate",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.sample_rate)))
        ("c,chans", "Number of channels",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.n_channels)))
        ("v,volume", "Signal volume, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.volume)))
        ("l,latency", "Ring buffer size, microseconds",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.io_latency_us)))
        ("p,periods", "Number of periods in io ring buffer",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.io_num_periods)))
        ;

    opts.add_options("Reporting")
        ("f,report-format", "Output Format: text|json",
         cxxopts::value<std::string>()->default_value("text"))
        ("report-sma", "Simple moving average window for latency reports",
         cxxopts::value<size_t>()->default_value(std::to_string(config.report_sma_window)))
        ;

    opts.add_options("Dumping")
        ("dump-out", "File to dump output stream (`-' for stdout)",
         cxxopts::value<std::string>())
        ("dump-in", "File to dump input stream (`-' for stdout)",
         cxxopts::value<std::string>())
        ("dump-compression", "Compress dumped samples by given ratio using SMA",
         cxxopts::value<size_t>()->default_value(std::to_string(config.dump_compression)))
        ;

    opts.add_options("Correlation-based latency estimation")
        ("impulse-period", "Impulse period, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.impulse_period)))
        ("impulse-peak-noise-ratio", "The peak-to-noise minimum ratio threshold",
         cxxopts::value<float>()->default_value(
             std::to_string(config.impulse_avg_2_peak_ration_threshold)))
        ("impulse-peak-window", "Peak detection window length, in samples",
         cxxopts::value<size_t>()->default_value(
             std::to_string(config.impulse_peak_detection_width)))
        ;

    opts.add_options("Step-based latency estimation")
        ("step-period", "Step period, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.step_period)))
        ("step-length", "Step length, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.step_length)))
        ("step-detection-window", "Step detection running maximum window, in samples",
         cxxopts::value<size_t>()->default_value(std::to_string(config.step_detection_window)))
        ("step-detection-threshold", "Step detection threshold, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.step_detection_threshold)))
        ;

    opts.add_options("Loss ratio estimation")
        ("signal-detection-window", "Signal detection running maximum window, in samples",
         cxxopts::value<size_t>()->default_value(std::to_string(config.signal_detection_window)))
        ("signal-detection-threshold", "Signal detection threshold, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.signal_detection_threshold)))
        ("glitch-detection-window", "Glitch detection running maximum window, in samples",
         cxxopts::value<size_t>()->default_value(std::to_string(config.glitch_detection_window)))
        ("glitch-detection-threshold", "Glitch detection threshold, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.glitch_detection_threshold)))
        ;

    std::string mode, format, input_dev, output_dev, input_dump, output_dump;

    try {
        auto res = opts.parse(argc, argv);

        if (res.count("help")) {
            std::cout << opts.help({
                "General",
                "I/O",
                "Reporting",
                "Dumping",
                "Correlation-based latency estimation",
                "Step-based latency estimation",
                "Loss ratio estimation",
            }) << std::endl;
            exit(0);
        }

        format = res["report-format"].as<std::string>();

        if (format != "text" && format != "json") {
            se_log_error("unknown --report-format value");
            exit(1);
        }

        mode = res["mode"].as<std::string>();

        if (mode != "noop" && mode != "latency_corr" &&  mode != "latency_step"
              && mode != "losses") {
            se_log_error("unknown --mode value");
            exit(1);
        }

        if (!res.count("output")) {
            se_log_error("missing --output device");
            exit(1);
        }

        if (!res.count("input")) {
            se_log_error("missing --input device");
            exit(1);
        }

        output_dev = res["output"].as<std::string>();
        input_dev = res["input"].as<std::string>();

        config.measurement_duration = res["duration"].as<float>();
        config.warmup_duration = res["warmup"].as<float>();

        config.sample_rate = res["rate"].as<unsigned int>();
        config.n_channels = res["chans"].as<unsigned int>();
        config.volume = res["volume"].as<float>();
        config.io_num_periods = res["periods"].as<unsigned int>();
        config.io_latency_us = res["latency"].as<unsigned int>();

        config.report_sma_window = res["report-sma"].as<size_t>();

        if (res.count("dump-out")) {
            output_dump = res["dump-out"].as<std::string>();
        }
        if (res.count("dump-in")) {
            input_dump = res["dump-in"].as<std::string>();
        }
        config.dump_compression = res["dump-compression"].as<size_t>();

        config.impulse_peak_detection_width = res["impulse-peak-window"].as<size_t>();
        config.impulse_avg_2_peak_ration_threshold = res["impulse-peak-noise-ratio"].as<float>();
        config.impulse_period = res["impulse-period"].as<float>();

        config.step_period = res["step-period"].as<float>();
        config.step_length = res["step-length"].as<float>();
        config.step_detection_window = res["step-detection-window"].as<size_t>();
        config.step_detection_threshold = res["step-detection-threshold"].as<float>();

        config.signal_detection_window = res["signal-detection-window"].as<size_t>();
        config.signal_detection_threshold = res["signal-detection-threshold"].as<float>();
        config.glitch_detection_window = res["glitch-detection-window"].as<size_t>();
        config.glitch_detection_threshold = res["glitch-detection-threshold"].as<float>();
    } catch (std::exception& exc) {
        se_log_error("%s", exc.what());
        exit(1);
    }

    AlsaWriter output_writer;

    if (!output_writer.open(config, output_dev.c_str())) {
        exit(1);
    }

    AlsaReader input_reader;

    if (!input_reader.open(config, input_dev.c_str())) {
        exit(1);
    }

    std::unique_ptr<IGenerator> generator;

    if (mode == "noop" || mode == "latency_step") {
        generator = std::make_unique<StepsGenerator>(config);
    } else if(mode == "latency_corr") {
        generator = std::make_unique<ImpulseGenerator>(config, impulse);
    } else {
        generator = std::make_unique<ContinuousGenerator>(config);
    }

    std::unique_ptr<IFormatter> formatter;

    if (format == "text") {
        formatter = std::make_unique<TextFormatter>();
    } else if (format == "json") {
        formatter = std::make_unique<JsonFormatter>();
    }

    std::unique_ptr<IEstimator> estimator;

    if (mode == "latency_step") {
        estimator = std::make_unique<StepsLatencyEstimator>(config, *formatter);
    } else if (mode == "latency_corr") {
        estimator = std::make_unique<CorrelationLatencyEstimator>(config, *formatter);
    } else if (mode == "losses") {
        estimator = std::make_unique<LossEstimator>(config, *formatter);
    }

    std::unique_ptr<IDumper> output_dumper;

    if (!output_dump.empty()) {
        auto dumper = std::make_unique<CsvDumper>(config);
        if (!dumper->open(output_dump.c_str())) {
            exit(1);
        }
        output_dumper = std::move(dumper);
    }

    std::unique_ptr<IDumper> input_dumper;

    if (!input_dump.empty()) {
        auto dumper = std::make_unique<CsvDumper>(config);
        if (!dumper->open(input_dump.c_str())) {
            exit(1);
        }
        input_dumper = std::move(dumper);
    }

    if (output_dumper) {
        output_dumper = std::make_unique<AsyncDumper>(std::move(output_dumper));
    }

    if (input_dumper) {
        input_dumper = std::make_unique<AsyncDumper>(std::move(input_dumper));
    }

    FramePool frame_pool(config);

    std::thread input_thread(input_loop, &config, &frame_pool,
            estimator.get(), &input_reader, input_dumper.get());

    output_loop(&config, &frame_pool, generator.get(), estimator.get(),  &output_writer,
        output_dumper.get());

    input_thread.join();

    return 0;
}
