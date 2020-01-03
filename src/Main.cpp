/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaReader.hpp"
#include "AlsaWriter.hpp"
#include "FileWriter.hpp"
#include "Log.hpp"
#include "Realtime.hpp"
#include "SignalGenerator.hpp"

#include <future>
#include <vector>

using namespace signal_estimator;

namespace {

void writer_loop(const Config& config, SignalGenerator& generator, AlsaWriter& writer,
    FileWriter& file) {
    set_realtime();

    std::vector<int16_t> buf(writer.period_size());

    for (size_t n = 0; n < config.total_samples() / buf.size(); n++) {
        generator.generate(&buf[0], buf.size());

        if (!writer.write(&buf[0], buf.size())) {
            exit(1);
        }

        file.write(&buf[0], buf.size());
    }
}

void reader_loop(const Config& config, AlsaReader& reader, FileWriter& file) {
    set_realtime();

    std::vector<int16_t> buf(reader.period_size());

    for (size_t n = 0; n < config.total_samples() / buf.size(); n++) {
        if (!reader.read(&buf[0], buf.size())) {
            exit(1);
        }

        file.write(&buf[0], buf.size());
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        log_error("usage: %s playback_device capture_device", argv[0]);
        exit(1);
    }

    Config config;

    AlsaWriter writer(config);

    if (!writer.open(argv[1])) {
        exit(1);
    }

    AlsaReader reader(config);

    if (!reader.open(argv[2])) {
        exit(1);
    }

    FileWriter writer_file;

    if (!writer_file.open("writer.txt")) {
        exit(1);
    }

    FileWriter reader_file;

    if (!reader_file.open("reader.txt")) {
        exit(1);
    }

    SignalGenerator generator(config);

    auto reader_thread = std::async(std::launch::async, [&]() {
        reader_loop(config, reader, reader_file);
    });

    writer_loop(config, generator, writer, writer_file);

    reader_thread.wait();

    return 0;
}
