/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "Frame.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class FileDumper {
public:
    FileDumper(const Config& config);
    ~FileDumper();

    FileDumper(const FileDumper&) = delete;
    FileDumper& operator=(const FileDumper&) = delete;

    bool open(const char* filename);
    void close();

    void write(Frame& frame);

private:
    void write_subframe_(nanoseconds_t ts, const sample_t* buf, size_t bufsz);

    void print_last_maybe_();

    const Config config_;

    FILE* fp_ {};

    unsigned long last_printed_ts_ {};
    unsigned long last_ts_ {};
    int last_val_ {};
};

} // namespace signal_estimator
