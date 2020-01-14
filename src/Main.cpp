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
    if (argc != 3) {
        log_error("usage: %s playback_device capture_device", argv[0]);
        exit(1);
    }

    Config config;

    AlsaWriter writer;

    if (!writer.open(config, argv[1])) {
        exit(1);
    }

    AlsaReader reader;

    if (!reader.open(config, argv[2])) {
        exit(1);
    }

    LatencyEstimator latency_estimator(config);

    FileDumper writer_file(config);

    if (!writer_file.open("writer.txt")) {
        exit(1);
    }

    FileDumper reader_file(config);

    if (!reader_file.open("reader.txt")) {
        exit(1);
    }

    SignalGenerator generator(config);

    auto reader_thread = std::async(std::launch::async, [&]() {
        reader_loop(config, latency_estimator, reader, reader_file);
    });

    writer_loop(config, generator, latency_estimator, writer, writer_file);

    reader_thread.wait();

    return 0;
}
