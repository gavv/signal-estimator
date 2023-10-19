// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "dumps/CsvDumper.hpp"
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
    win_size_ = std::max(config.dump_compression, size_t(1));

    for (size_t ch = 0; ch < config_.n_channels; ch++) {
        win_avg_.emplace_back(win_size_);
    }

    // upper bound
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
        se_log_error("can't open output file {}: {}", filename, strerror(errno));
        return false;
    }

    fprintf(fp_, "# type,timestamp,ch1,ch2,...\n");
    fflush(fp_);

    return true;
}

void CsvDumper::close() {
    if (fp_) {
        if (fp_ != stdout) {
            fclose(fp_);
        }
        fp_ = nullptr;
    }
}

void CsvDumper::write(FramePtr frame) {
    if (!frame) {
        return;
    }

    if (!fp_) {
        return;
    }

    print_(*frame);
}

void CsvDumper::print_(const Frame& frame) {
    for (size_t n = 0; n < frame.size(); n++) {
        if (n % config_.n_channels == 0) {
            if (win_pos_ == win_size_) {
                print_line_(frame.type(), *win_time_);

                win_time_ = {};
                win_pos_ = 0;
            }
            if (!win_time_) {
                win_time_ = frame.hw_sample_time(n)
                    + config_.samples_to_ns(win_size_ / 2 * config_.n_channels);
            }
            win_pos_++;
        }
        win_avg_[n % config_.n_channels].add(frame[n]);
    }
}

void CsvDumper::print_line_(FrameType type, nanoseconds_t timestamp) {
    size_t off = 0;

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "%s,%lld",
        type == FrameType::Output ? "o" : "i", (long long)timestamp);

    for (size_t ch = 0; ch < config_.n_channels; ch++) {
        off += (size_t)snprintf(
            buf_.data() + off, buf_.size() - off, ",%lld", (long long)win_avg_[ch].get());
    }

    off += snprintf(buf_.data() + off, buf_.size() - off, "\n");

    fprintf(fp_, "%s", buf_.data());
    fflush(fp_);
}

} // namespace signal_estimator
