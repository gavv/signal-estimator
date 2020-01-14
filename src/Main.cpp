/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaReader.hpp"
#include "AlsaWriter.hpp"
#include "FileDumper.hpp"
#include "LatencyEstimator.hpp"
#include "Log.hpp"
#include "Realtime.hpp"
#include "SignalGenerator.hpp"
#include "Time.hpp"

#include <cxxopts.hpp>

#include <future>
#include <vector>

using namespace signal_estimator;

namespace {

void writer_loop(const Config& config, SignalGenerator& generator,
    LatencyEstimator& latency_estimator, AlsaWriter& writer, FileDumper& dumper) {
    set_realtime();

    std::vector<int16_t> buf(config.period_size);

    for (size_t n = 0; n < config.total_samples() / buf.size(); n++) {
        generator.generate(&buf[0], buf.size());

        if (!writer.write(&buf[0], buf.size())) {
            exit(1);
        }

        const auto ts = monotonic_timestamp_ns();

        latency_estimator.add_output(ts, &buf[0], buf.size());

        dumper.write(Dir::Playback, ts, &buf[0], buf.size());
    }
}

void reader_loop(const Config& config, LatencyEstimator& latency_estimator,
    AlsaReader& reader, FileDumper& dumper) {
    set_realtime();

    std::vector<int16_t> buf(config.period_size);

    for (size_t n = 0; n < config.total_samples() / buf.size(); n++) {
        if (!reader.read(&buf[0], buf.size())) {
            exit(1);
        }

        const auto ts = monotonic_timestamp_ns();

        latency_estimator.add_input(ts, &buf[0], buf.size());

        dumper.write(Dir::Recording, ts, &buf[0], buf.size());
    }
}

} // namespace

int main(int argc, char** argv) {
    Config config;

    cxxopts::Options opts(
        "signal-estimator", "Measure characteristics of a looped signal");

    opts.add_options("General")
        ("h,help", "Print help message and exit")
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

    std::string input, output, input_dump, output_dump;

    try {
        auto res = opts.parse(argc, argv);

        if (res.count("help")) {
            std::cout << opts.help({
                    "General",
                    "Reporting",
                    "Latency estimation",
                    "File dumping",
                }) << std::endl;
            exit(0);
        }

        if (!res.count("output")) {
            log_error("missing --output device");
            exit(1);
        }

        if (!res.count("input")) {
            log_error("missing --output device");
            exit(1);
        }

        output = res["output"].as<std::string>();
        input = res["input"].as<std::string>();

        config.sample_rate = res["rate"].as<unsigned int>();
        config.n_channels = res["chans"].as<unsigned int>();
        config.n_periods = res["periods"].as<unsigned int>();
        config.latency_us = res["latency"].as<unsigned int>();
        config.duration = res["duration"].as<float>();

        config.sma_window = res["sma"].as<size_t>();

        config.strike_period = res["strike-period"].as<float>();
        config.strike_length = res["strike-length"].as<float>();
        config.strike_window = res["strike-window"].as<size_t>();
        config.strike_threshold = res["strike-threshold"].as<float>();

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
        log_error("%s", exc.what());
        exit(1);
    }

    AlsaWriter writer;

    if (!writer.open(config, output.c_str())) {
        exit(1);
    }

    AlsaReader reader;

    if (!reader.open(config, input.c_str())) {
        exit(1);
    }

    LatencyEstimator latency_estimator(config);

    FileDumper writer_file(config);

    if (!output_dump.empty()) {
        if (!writer_file.open(output_dump.c_str())) {
            exit(1);
        }
    }

    FileDumper reader_file(config);

    if (!input_dump.empty()) {
        if (!reader_file.open(input_dump.c_str())) {
            exit(1);
        }
    }

    SignalGenerator generator(config);

    auto reader_thread = std::async(std::launch::async, [&]() {
        reader_loop(config, latency_estimator, reader, reader_file);
    });

    writer_loop(config, generator, latency_estimator, writer, writer_file);

    reader_thread.wait();

    return 0;
}
