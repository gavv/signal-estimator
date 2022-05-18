// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

// dump samples with their timestamps to a text file
// to reduce file size, dumps only one maximum value among every N samples
// to reduce file size even more, rounds every dumped value and dumps
// new sample only if its rounded value was changed
class FileDumper {
public:
    FileDumper(const Config& config);
    ~FileDumper();

    FileDumper(const FileDumper&) = delete;
    FileDumper& operator=(const FileDumper&) = delete;

    bool open(const char* filename);
    void close();

    void write(Frame& frame);
    void print_output_();
    void print_input_();

private:
    static constexpr nanoseconds_t MaxPrintDelay = 10 * Millisecond;

    void write_subframe_(
        nanoseconds_t ts, const sample_t* buf, size_t bufsz, const IOType type);

    void print_last_maybe_(const IOType type);

    const Config config_;

    FILE* fp_ {};

    nanoseconds_t last_printed_ts_ {};
    nanoseconds_t last_ts_ {};
    sample_t last_val_ {};
};

} // namespace signal_estimator
