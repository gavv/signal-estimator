// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Formatter.hpp"

#include "core/Log.hpp"
#include "run/Runner.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <map>

using namespace signal_estimator;

int main(int argc, char** argv) {
    Config config;
    std::string mode = "latency_corr";
    std::string report_format = "text";
    int verbosity = 0;

    CLI::App app { "Measure characteristics of a looped back signal",
        "signal-estimator" };

    app.formatter(std::make_shared<Formatter>());

    app.add_flag("-v,--verbose", verbosity,
        "increase verbosity level (may be used multiple times)");

    auto control_opts = app.add_option_group("Control options");

    control_opts
        ->add_option("-m,--mode", mode,
            "Operation mode: latency_corr|latency_step|losses|io_jitter")
        ->default_str(mode);
    control_opts->add_option("-o,--output", config.output_dev, "Output device name");
    control_opts->add_option("-i,--input", config.input_devs, "Input device name")
        ->expected(0, -1);
    control_opts->add_flag("--diff", config.diff_inputs,
        "Measure difference between input devices (requires exactly two inputs)");
    control_opts
        ->add_option("-d,--duration", config.measurement_duration,
            "Limit measurement duration, seconds (zero for no limit)")
        ->default_val(config.measurement_duration);
    control_opts
        ->add_option("-w,--warmup", config.warmup_duration,
            "Warmup duration, seconds (zero for no warmup)")
        ->default_val(config.warmup_duration);

    auto io_opts = app.add_option_group("I/O options");

    io_opts->add_option("-r,--rate", config.sample_rate, "Sample rate, Hz")
        ->default_val(config.sample_rate);
    io_opts->add_option("-c,--chans", config.channel_count, "Number of channels")
        ->default_val(config.channel_count);
    io_opts->add_option("-g,--gain", config.gain, "Signal gain, from 0 to 1")
        ->default_val(config.gain);
    io_opts
        ->add_option("--in-latency", config.requested_input_latency_us,
            "Input ring buffer size, microseconds")
        ->default_val(config.requested_input_latency_us);
    io_opts
        ->add_option("--in-periods", config.requested_input_period_count,
            "Number of periods in input ring buffer")
        ->default_val(config.requested_input_period_count);
    io_opts
        ->add_option("--out-latency", config.requested_output_latency_us,
            "Output ring buffer size, microseconds")
        ->default_val(config.requested_output_latency_us);
    io_opts
        ->add_option("--out-periods", config.requested_output_period_count,
            "Number of periods in output ring buffer")
        ->default_val(config.requested_output_period_count);

    auto report_opts = app.add_option_group("Report options");

    report_opts
        ->add_option("-f,--report-format", report_format, "Report format: text|json")
        ->default_str(report_format);
    report_opts
        ->add_option("--report-sma", config.report_sma_window,
            "Simple moving average window for latency reports")
        ->default_val(config.report_sma_window);

    auto dump_opts = app.add_option_group("Dump options");

    dump_opts->add_option("--dump-out", config.output_dump,
        "File to dump output stream (\"-\" for stdout)");
    dump_opts->add_option(
        "--dump-in", config.input_dump, "File to dump input stream (\"-\" for stdout)");
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

    auto iojitter_opts = app.add_option_group("I/O jitter estimation options");

    iojitter_opts
        ->add_option("--io-jitter-window", config.io_jitter_window,
            "I/O jitter detection window, number of periods")
        ->default_val(config.io_jitter_window);
    iojitter_opts
        ->add_option("--io-jitter-percentile", config.io_jitter_percentile,
            "I/O jitter percentile, from 1 to 100")
        ->default_val(config.io_jitter_percentile);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    // parse mode
    const std::map<std::string, Mode> mode_map {
        { "latency_corr", Mode::LatencyCorr },
        { "latency_step", Mode::LatencyStep },
        { "losses", Mode::Losses },
        { "io_jitter", Mode::IOJitter },
    };

    if (!mode_map.count(mode)) {
        std::cerr << "--mode: Invalid value\n"
                  << "Run with --help for more information.\n";
        return EXIT_FAILURE;
    }

    config.mode = mode_map.at(mode);

    // parse format
    const std::map<std::string, Format> format_map {
        { "text", Format::Text },
        { "json", Format::Json },
    };

    if (!format_map.count(report_format)) {
        std::cerr << "--report-format: Invalid value\n"
                  << "Run with --help for more information.\n";
        return EXIT_FAILURE;
    }

    config.report_format = format_map.at(report_format);

    // validate devices
    const bool have_output = !config.output_dev.empty();
    const bool have_input = !config.input_devs.empty();

    if (config.mode != Mode::IOJitter) {
        if (!have_output || !have_input) {
            std::cerr << "--mode " << mode << " requires one --output device AND\n"
                      << "one or more --input devices\n"
                      << "Run with --help for more information.\n";
            return EXIT_FAILURE;
        }
    } else {
        if ((have_output && have_input) || (!have_output && !have_input)) {
            std::cerr << "--mode " << mode << " requires either one --output device OR\n"
                      << "one or more --input devices\n"
                      << "Run with --help for more information.\n";
            return EXIT_FAILURE;
        }
    }

    if (config.diff_inputs) {
        if (config.mode == Mode::IOJitter) {
            std::cerr << "--mode " << mode << " does not support --diff option\n"
                      << "Run with --help for more information.\n";
            return EXIT_FAILURE;
        }
        if (config.input_devs.size() != 2) {
            std::cerr << "--diff requires exactly two --input devices\n"
                      << "Run with --help for more information.\n";
            return EXIT_FAILURE;
        }
    }

    // setup and run
    init_log(verbosity);

    const int code = [&]() {
        Runner runner(config);

        if (!runner.start()) {
            return EXIT_FAILURE;
        }

        runner.wait();

        if (runner.failed()) {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }();

    if (code != 0) {
        se_log_debug("exiting with code {}", code);
    }

    return code;
}
