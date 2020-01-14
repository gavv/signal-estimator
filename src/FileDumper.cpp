/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "FileDumper.hpp"
#include "Log.hpp"
#include "Time.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>

namespace signal_estimator {

FileDumper::FileDumper(const Config& config)
    : config_(config) {
}

FileDumper::~FileDumper() {
    close();
}

bool FileDumper::open(const char* filename) {
    fp_ = fopen(filename, "w");

    if (!fp_) {
        log_error("can't open output file %s: %s", filename, strerror(errno));
        return false;
    }

    return true;
}

void FileDumper::close() {
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }
}

void FileDumper::write(Dir dir, nanoseconds_t ts, const int16_t* buf, size_t bufsz) {
    if (!fp_) {
        return;
    }

    size_t off = 0;

    while (off < bufsz) {
        const nanoseconds_t frame_ts = ts + sample_offset(config_, dir, off);

        const auto frame_data = buf + off;
        const auto frame_sz = std::min(config_.dump_frame, bufsz - off);

        write_frame_(frame_ts, frame_data, frame_sz);

        off += frame_sz;
    }
}

void FileDumper::write_frame_(nanoseconds_t ts, const int16_t* buf, size_t bufsz) {
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

    last_ts_ = ts;
    last_val_ = new_val;

    if (changed) {
        print_last_maybe_();
    }
}

void FileDumper::print_last_maybe_() {
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
