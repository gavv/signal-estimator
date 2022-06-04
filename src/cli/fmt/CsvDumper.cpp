// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "fmt/CsvDumper.hpp"
#include "core/Frame.hpp"
#include "core/Log.hpp"
#include "core/Time.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace signal_estimator {

CsvDumper::CsvDumper(const Config& config)
    : config_(config) {
    const auto win_size = std::max(config.dump_compression, size_t(1));

    for (size_t ch = 0; ch < config_.n_channels; ch++) {
        mavg_.emplace_back(win_size);
    }

    buf_.resize(128 + 24 * config_.n_channels);
}

CsvDumper::~CsvDumper() {
    close();
}

bool CsvDumper::open(const char* filename) {
    if (strcmp(filename, "-") == 0) {
        fp_ = stdout;
    } else {
        fp_ = fopen(filename, "w");
    }

    if (!fp_) {
        se_log_error("can't open output file %s: %s", filename, strerror(errno));
        return false;
    }

    fprintf(fp_, "# type,timestamp,ch1,ch2,...\n");
    fflush(fp_);

    return true;
}

void CsvDumper::close() {
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }
}

void CsvDumper::write(std::shared_ptr<Frame> frame) {
    if (!frame) {
        return;
    }

    if (!fp_) {
        return;
    }

    for (size_t n = 0; n < frame->size(); n++) {
        if (n % config_.n_channels == 0) {
            if (pos_ != 0 && pos_ % std::max(config_.dump_compression, size_t(1)) == 0) {
                print_line_(frame->io_type(), frame->sw_frame_time());
            }
            pos_++;
        }
        mavg_[n % config_.n_channels].add((*frame)[n]);
    }
}

void CsvDumper::print_line_(IOType type, nanoseconds_t timestamp) {
    size_t off = 0;

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "%s,%llu",
        type == IOType::Output ? "o" : "i", (unsigned long long)timestamp);

    for (size_t ch = 0; ch < config_.n_channels; ch++) {
        off += (size_t)snprintf(
            buf_.data() + off, buf_.size() - off, ",%lld", (long long)mavg_[ch].get());
    }

    off += snprintf(buf_.data() + off, buf_.size() - off, "\n");

    fprintf(fp_, "%s", buf_.data());
    fflush(fp_);
}

} // namespace signal_estimator
