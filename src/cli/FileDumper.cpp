/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "FileDumper.hpp"
#include "Log.hpp"
#include "Time.hpp"
#include "Frame.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>

namespace signal_estimator {

namespace {

sample_t find_max(const sample_t* buf, size_t bufsz) {
    auto max_val = buf[0];

    for (size_t n = 1; n < bufsz; n++) {
        if (std::abs(buf[n]) > std::abs(max_val)) {
            max_val = buf[n];
        }
    }

    return max_val;
}

} // namespace

FileDumper::FileDumper(const Config& config)
    : config_(config) {
}

FileDumper::~FileDumper() {
    close();
}

bool FileDumper::open(const char* filename) {
    if (strcmp(filename,"-") == 0){
       fp_ = stdout;
    }
    else
    fp_ = fopen(filename, "w");

    if (!fp_) {
        se_log_error("can't open output file %s: %s", filename, strerror(errno));
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

void FileDumper::write(Frame& frame) {
    if (!fp_) {
        return;
    }

    for (size_t off = 0; off < frame.size();) {
        const auto subframe_ts = frame.hw_sample_time(off);

        const auto subframe_data = frame.data() + off;
        const auto subframe_sz = std::min(config_.dump_frame, frame.size() - off);

        write_subframe_(subframe_ts, subframe_data, subframe_sz, frame.get_io_type());

        off += subframe_sz;
    }
}

  void FileDumper::write_subframe_(nanoseconds_t ts, const sample_t* buf, size_t bufsz, const IOType type) {
    const int new_val
        = int(find_max(buf, bufsz) / config_.dump_rounding * config_.dump_rounding);

    const bool changed = (new_val != last_val_);

    if (changed) {
        print_last_maybe_(type);
    }


    last_ts_ = ts;
    last_val_ = new_val;

    if (changed) {
        print_last_maybe_(type);
    }

}

void FileDumper::print_last_maybe_(const IOType type) {
    if (last_ts_ == 0) {
        return;
    }

    if (last_printed_ts_ == last_ts_) {
        return;
    }
    if (type == IOType::Output && fp_ == stdout)
        fprintf(fp_, "out %lu %d\n", last_ts_, last_val_);
    else if (fp_ == stdout)
        fprintf(fp_, "in %lu %d\n", last_ts_, last_val_);
    else
        fprintf(fp_, "%lu %d\n", last_ts_, last_val_);
    fflush(fp_);

    last_printed_ts_ = last_ts_;
}

} // namespace signal_estimator
