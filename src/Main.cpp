/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaReader.hpp"
#include "AlsaWriter.hpp"
#include "ContinuousGenerator.hpp"
#include "FileDumper.hpp"
#include "LatencyEstimator.hpp"
#include "LossEstimator.hpp"
#include "Log.hpp"
#include "Realtime.hpp"
#include "StrikeGenerator.hpp"
#include "Time.hpp"

#include <cxxopts.hpp>

#include <future>
#include <memory>

using namespace signal_estimator;

namespace {

void output_loop(const Config& config, IGenerator& generator, IEstimator* estimator,
    AlsaWriter& writer, FileDumper* dumper) {
    set_realtime();

    Frame frame(config);

    for (size_t n = 0; n < config.total_samples() / frame.size(); n++) {
        generator.generate(frame);

        if (!writer.write(frame)) {
            exit(1);
        }

        if (estimator) {
            estimator->add_output(frame);
        }

        if (dumper) {
            dumper->write(frame);
        }
    }
}

void input_loop(
    const Config& config, IEstimator* estimator, AlsaReader& reader, FileDumper* dumper) {
    set_realtime();

    Frame frame(config);

    for (size_t n = 0; n < config.total_samples() / frame.size(); n++) {
        if (!reader.read(frame)) {
            exit(1);
        }

        if (estimator) {
            estimator->add_input(frame);
        }

        if (dumper) {
            dumper->write(frame);
        }
    }
}

} // namespace

int main(int argc, char** argv) {
    Config config;

    cxxopts::Options opts(
        "signal-estimator", "Measure characteristics of a looped back signal");

    opts.add_options("General")
        ("h,help", "Print help message and exit")
        ("m,mode", "Mode: noop|latency|losses",
         cxxopts::value<std::string>()->default_value("latency"))
        ("o,output", "Output device",
         cxxopts::value<std::string>())
        ("i,input", "Input device",
         cxxopts::value<std::string>())
        ("r,rate", "Sample rate",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.sample_rate)))
        ("c,chans", "Number of channels",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.n_channels)))
        ("p,periods", "Number of periods in buffer",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.n_periods)))
        ("l,latency", "Input or output latency, us",
         cxxopts::value<unsigned int>()->default_value(std::to_string(config.latency_us)))
        ("d,duration", "Test duration, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.duration)))
        ("v,volume", "Test signal volume, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.volume)))
        ;

    opts.add_options("Reporting")
        ("sma", "Simple moving average window in reports",
         cxxopts::value<size_t>()->default_value(std::to_string(config.sma_window)))
        ;

    opts.add_options("Latency estimation")
        ("strike-period", "Strike period, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.strike_period)))
        ("strike-length", "Strike length, seconds",
         cxxopts::value<float>()->default_value(std::to_string(config.strike_length)))
        ("strike-window", "Strike detection running maximum window, in samples",
         cxxopts::value<size_t>()->default_value(std::to_string(config.strike_window)))
        ("strike-threshold", "Strike detection threshold, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.strike_threshold)))
        ;

    opts.add_options("Loss ratio estimation")
        ("glitch-window", "Glitch detection running maximum window, in samples",
         cxxopts::value<size_t>()->default_value(std::to_string(config.glitch_window)))
        ("glitch-threshold", "Glitch detection threshold, from 0 to 1",
         cxxopts::value<float>()->default_value(std::to_string(config.glitch_threshold)))
        ;

    opts.add_options("File dumping")
        ("O,dump-output", "File to dump output stream",
         cxxopts::value<std::string>())
        ("I,dump-input", "File to dump input stream",
         cxxopts::value<std::string>())
        ("dump-frame", "Frame size",
         cxxopts::value<size_t>()->default_value(std::to_string(config.dump_frame)))
        ("dump-rounding", "Rounding",
         cxxopts::value<size_t>()->default_value(std::to_string(config.dump_rounding)))
        ;

    std::string mode, input_dev, output_dev, input_dump, output_dump;

    try {
        auto res = opts.parse(argc, argv);

        if (res.count("help")) {
            std::cout << opts.help({
                    "General",
                    "Reporting",
                    "Latency estimation",
                    "Loss ratio estimation",
                    "File dumping",
                }) << std::endl;
            exit(0);
        }

        mode = res["mode"].as<std::string>();

        if (mode != "noop" && mode != "latency" && mode != "losses") {
            se_log_error("unknown --mode value");
            exit(1);
        }

        if (!res.count("output")) {
            se_log_error("missing --output device");
            exit(1);
        }

        if (!res.count("input")) {
            se_log_error("missing --output device");
            exit(1);
        }

        output_dev = res["output"].as<std::string>();
        input_dev = res["input"].as<std::string>();

        config.sample_rate = res["rate"].as<unsigned int>();
        config.n_channels = res["chans"].as<unsigned int>();
        config.n_periods = res["periods"].as<unsigned int>();
        config.latency_us = res["latency"].as<unsigned int>();
        config.duration = res["duration"].as<float>();
        config.volume = res["volume"].as<float>();

        config.sma_window = res["sma"].as<size_t>();

        config.strike_period = res["strike-period"].as<float>();
        config.strike_length = res["strike-length"].as<float>();
        config.strike_window = res["strike-window"].as<size_t>();
        config.strike_threshold = res["strike-threshold"].as<float>();

        config.glitch_window = res["glitch-window"].as<size_t>();
        config.glitch_threshold = res["glitch-threshold"].as<float>();

        config.dump_frame = res["dump-frame"].as<size_t>();
        config.dump_rounding = res["dump-rounding"].as<size_t>();

        if (res.count("dump-output")) {
            output_dump = res["dump-output"].as<std::string>();
        }

        if (res.count("dump-input")) {
            input_dump = res["dump-input"].as<std::string>();
        }
    }
    catch (std::exception& exc) {
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

    if (mode == "noop" || mode == "latency") {
        generator = std::make_unique<StrikeGenerator>(config);
    } else {
        generator = std::make_unique<ContinuousGenerator>(config);
    }

    std::unique_ptr<IEstimator> estimator;

    if (mode == "latency") {
        estimator = std::make_unique<LatencyEstimator>(config);
    } else if (mode == "losses") {
        estimator = std::make_unique<LossEstimator>(config);
    }

    std::unique_ptr<FileDumper> output_dumper;

    if (!output_dump.empty()) {
        output_dumper = std::make_unique<FileDumper>(config);

        if (!output_dumper->open(output_dump.c_str())) {
            exit(1);
        }
    }

    std::unique_ptr<FileDumper> input_dumper;

    if (!input_dump.empty()) {
        input_dumper = std::make_unique<FileDumper>(config);

        if (!input_dumper->open(input_dump.c_str())) {
            exit(1);
        }
    }

    auto input_thread = std::async(std::launch::async, [&]() {
        input_loop(config, estimator.get(), input_reader, input_dumper.get());
    });

    output_loop(config, *generator, estimator.get(), output_writer, output_dumper.get());

    input_thread.wait();

    return 0;
}
