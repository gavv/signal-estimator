/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class FileWriter {
public:
    FileWriter() = default;
    ~FileWriter();

    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

    bool open(const char* filename);
    void close();

    void write(const int16_t* buf, size_t bufsz);

private:
    void print_last_maybe_();

    FILE* fp_ {};

    unsigned long last_printed_ts_ {};
    unsigned long last_ts_ {};
    int last_val_ {};
};

} // namespace signal_estimator
