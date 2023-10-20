// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Formatter.hpp"

#include "core/FramePool.hpp"
#include "core/Log.hpp"
#include "core/Realtime.hpp"
#include "dumps/AsyncDumper.hpp"
#include "dumps/CsvDumper.hpp"
#include "dumps/IDumper.hpp"
#include "io/AlsaReader.hpp"
#include "io/AlsaWriter.hpp"
#include "io/IDeviceReader.hpp"
#include "io/IDeviceWriter.hpp"
#include "processing/ContinuousGenerator.hpp"
#include "processing/CorrelationLatencyEstimator.hpp"
#include "processing/Impulse.hpp"
#include "processing/ImpulseGenerator.hpp"
#include "processing/LossEstimator.hpp"
#include "processing/StepsGenerator.hpp"
#include "processing/StepsLatencyEstimator.hpp"
#include "reports/IReporter.hpp"
#include "reports/JsonReporter.hpp"
#include "reports/TextReporter.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace signal_estimator;

namespace {

void output_loop(const Config* config, FramePool* frame_pool, IGenerator* generator,
    IEstimator* estimator, IDeviceWriter* writer, IDumper* dumper) {
    make_realtime();

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

void input_loop(const Config* config, FramePool* frame_pool, IEstimator* estimator,
    IDeviceReader* reader, IDumper* dumper) {
    make_realtime();

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
    std::string mode = "latency_corr";
    std::string format = "text";
    std::string output_dev, input_dev;
    std::string output_dump, input_dump;
    int verbosity = 0;

    CLI::App app { "Measure characteristics of a looped back signal",
        "signal-estimator" };

    app.formatter(std::make_shared<Formatter>());

    app.add_flag("-v,--verbose", verbosity,
        "increase verbosity level (may be used multiple times)");

    auto control_opts = app.add_option_group("Control options");

    control_opts
        ->add_option(
            "-m,--mode", mode, "Operation mode: latency_corr|latency_step|losses")
        ->default_str(mode);
    control_opts->add_option("-o,--output", output_dev, "Output device name")->required();
    control_opts->add_option("-i,--input", input_dev, "Input device name")->required();
    control_opts
        ->add_option(
            "-d,--duration", config.measurement_duration, "Measurement duration, seconds")
        ->default_val(config.measurement_duration);
    control_opts
        ->add_option("-w,--warmup", config.warmup_duration, "Warmup duration, seconds")
        ->default_val(config.warmup_duration);

    auto io_opts = app.add_option_group("I/O options");

    io_opts->add_option("-r,--rate", config.sample_rate, "Sample rate, Hz")
        ->default_val(config.sample_rate);
    io_opts->add_option("-c,--chans", config.n_channels, "Number of channels")
        ->default_val(config.n_channels);
    io_opts->add_option("-v,--volume", config.volume, "Signal volume, from 0 to 1")
        ->default_val(config.volume);
    io_opts
        ->add_option(
            "-l,--latency", config.io_latency_us, "Ring buffer size, microseconds")
        ->default_val(config.io_latency_us);
    io_opts
        ->add_option(
            "-p,--periods", config.io_num_periods, "Number of periods in ring buffer")
        ->default_val(config.io_num_periods);

    auto report_opts = app.add_option_group("Report options");

    report_opts->add_option("-f,--report-format", format, "Report format: text|json")
        ->default_str(format);
    report_opts
        ->add_option("--report-sma", config.report_sma_window,
            "Simple moving average window for latency reports")
        ->default_val(config.report_sma_window);

    auto dump_opts = app.add_option_group("Dump options");

    dump_opts->add_option(
        "--dump-out", output_dump, "File to dump output stream (`-' for stdout)");
    dump_opts->add_option(
        "--dump-in", input_dump, "File to dump input stream (`-' for stdout)");
    dump_opts
        ->add_option("--dump-compression", config.dump_compression,
            "Compress dumped samples by given ratio using SMA")
        ->default_val(config.dump_compression);

    auto corr_opts = app.add_option_group("Correlation-based latency estimation options");

    corr_opts
        ->add_option("--impulse-period", config.impulse_period, "Impulse period, seconds")
        ->default_val(config.impulse_period);
    corr_opts
        ->add_option("--impulse-peak-noise-ratio",
            config.impulse_avg_2_peak_ration_threshold,
            "The peak-to-noise minimum ratio threshold")
        ->default_val(config.impulse_avg_2_peak_ration_threshold);
    corr_opts
        ->add_option("--impulse-peak-window", config.impulse_peak_detection_width,
            "Peak detection window length, samples")
        ->default_val(config.impulse_peak_detection_width);

    auto step_opts = app.add_option_group("Step-based latency estimation options");

    step_opts->add_option("--step-period", config.step_period, "Step period, seconds")
        ->default_val(config.step_period);
    step_opts->add_option("--step-length", config.step_length, "Step length, seconds")
        ->default_val(config.step_length);
    step_opts
        ->add_option("--step-detection-window", config.step_detection_window,
            "Step detection running maximum window, samples")
        ->default_val(config.step_detection_window);
    step_opts
        ->add_option("--step-detection-threshold", config.step_detection_threshold,
            "Step detection threshold, from 0 to 1")
        ->default_val(config.step_detection_threshold);

    auto loss_opts = app.add_option_group("Loss ratio estimation options");

    loss_opts
        ->add_option("--signal-detection-window", config.signal_detection_window,
            "Signal detection running maximum window, samples")
        ->default_val(config.signal_detection_window);
    loss_opts
        ->add_option("--signal-detection-threshold", config.signal_detection_threshold,
            "Signal detection threshold, from 0 to 1")
        ->default_val(config.signal_detection_threshold);
    loss_opts
        ->add_option("--glitch-detection-window", config.glitch_detection_window,
            "Glitch detection running maximum window, samples")
        ->default_val(config.glitch_detection_window);
    loss_opts
        ->add_option("--glitch-detection-threshold", config.glitch_detection_threshold,
            "Glitch detection threshold, from 0 to 1")
        ->default_val(config.glitch_detection_threshold);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if (mode != "latency_corr" && mode != "latency_step" && mode != "losses") {
        std::cerr << "invalid --mode\n";
        std::cerr << "Run with --help for more information.\n";
        exit(1);
    }

    if (format != "text" && format != "json") {
        std::cerr << "invalid --report-format\n";
        std::cerr << "Run with --help for more information.\n";
        exit(1);
    }

    init_log(verbosity);

    AlsaWriter output_writer;

    if (!output_writer.open(config, output_dev.c_str())) {
        exit(1);
    }

    AlsaReader input_reader;

    if (!input_reader.open(config, input_dev.c_str())) {
        exit(1);
    }

    FramePool frame_pool(config);

    std::unique_ptr<IGenerator> generator;

    if (mode == "latency_step") {
        generator = std::make_unique<StepsGenerator>(config);
    } else if (mode == "latency_corr") {
        generator = std::make_unique<ImpulseGenerator>(config, impulse);
    } else if (mode == "losses") {
        generator = std::make_unique<ContinuousGenerator>(config);
    }

    std::unique_ptr<IReporter> reporter;

    if (format == "text") {
        reporter = std::make_unique<TextReporter>();
    } else if (format == "json") {
        reporter = std::make_unique<JsonReporter>();
    }

    std::unique_ptr<IEstimator> estimator;

    if (mode == "latency_step") {
        estimator = std::make_unique<StepsLatencyEstimator>(config, *reporter);
    } else if (mode == "latency_corr") {
        estimator = std::make_unique<CorrelationLatencyEstimator>(config, *reporter);
    } else if (mode == "losses") {
        estimator = std::make_unique<LossEstimator>(config, *reporter);
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

    std::thread input_thread(input_loop, &config, &frame_pool, estimator.get(),
        &input_reader, input_dumper.get());

    output_loop(&config, &frame_pool, generator.get(), estimator.get(), &output_writer,
        output_dumper.get());

    input_thread.join();

    return 0;
}
