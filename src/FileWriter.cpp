/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "FileWriter.hpp"
#include "Log.hpp"
#include "Time.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>

namespace signal_estimator {

FileWriter::~FileWriter() {
    close();
}

bool FileWriter::open(const char* filename) {
    fp_ = fopen(filename, "w");

    if (!fp_) {
        log_error("can't open output file %s: %s", filename, strerror(errno));
        return false;
    }

    return true;
}

void FileWriter::close() {
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }
}

void FileWriter::write(const int16_t* buf, size_t bufsz) {
    if (!fp_ || bufsz == 0) {
        return;
    }

    const auto new_ts = monotonic_timestamp();

    int16_t max_val = buf[0];

    for (size_t n = 1; n < bufsz; n++) {
        if (std::abs(buf[n]) > std::abs(max_val)) {
            max_val = buf[n];
        }
    }

    int new_val = int(max_val / 10 * 10);

    const bool changed = (new_val != last_val_);

    if (changed) {
        print_last_maybe_();
    }

    last_ts_ = new_ts;
    last_val_ = new_val;

    if (changed) {
        print_last_maybe_();
    }
}

void FileWriter::print_last_maybe_() {
    if (last_ts_ == 0) {
        return;
    }

    if (last_printed_ts_ == last_ts_) {
        return;
    }

    fprintf(fp_, "%lu %d\n", last_ts_, last_val_);
    fflush(fp_);

    last_printed_ts_ = last_ts_;
}

} // namespace signal_estimator
