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
#include <regex>

namespace signal_estimator {

namespace {

std::string quote(const std::string& str) {
    std::string quoted_str = str;

    if (quoted_str.find(',') != std::string::npos
        || quoted_str.find('"') != std::string::npos
        || quoted_str.find('\n') != std::string::npos
        || quoted_str.find('\r') != std::string::npos) {
        quoted_str = std::regex_replace(quoted_str, std::regex("\""), "\"\"");
        quoted_str = "\"" + quoted_str + "\"";
    }

    return quoted_str;
}

} // namespace

CsvDumper::CsvDumper(const Config& config)
    : config_(config) {
    // upper bound
    buf_.resize(256 + 24 * config_.channel_count);
}

CsvDumper::~CsvDumper() {
    close();
}

bool CsvDumper::open(const std::string& filename) {
    if (filename == "-") {
        fp_ = stdout;
    } else {
        fp_ = fopen(filename.c_str(), "w");
    }

    if (!fp_) {
        se_log_error("can't open output file {}: {}", filename, strerror(errno));
        return false;
    }

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

    DeviceState& dev = devices_[frame->dir()][frame->dev_index()];

    if (dev.win_size == 0) {
        device_init_(dev, *frame);
    }

    device_add_(dev, *frame);
}

void CsvDumper::device_init_(DeviceState& dev, const Frame& frame) {
    dev.dir = frame.dir();

    if (config_.show_device_names) {
        dev.name = quote(frame.dev_info().short_name);
    }

    // if dump_compression is 0 or 1, SMA window is 1, but has no effect
    dev.win_size = std::max(config_.dump_compression, (size_t)1);

    for (size_t ch = 0; ch < config_.channel_count; ch++) {
        dev.win_avg.emplace_back(dev.win_size);
    }
}

void CsvDumper::device_add_(DeviceState& dev, const Frame& frame) {
    for (size_t ns = 0; ns < frame.size(); ns += config_.channel_count) {
        // add samples to SMA window
        for (size_t ch = 0; ch < config_.channel_count; ch++) {
            dev.win_avg[ch].add(frame[ns + ch]);
        }

        if (dev.win_pos == 0) {
            // remember time of the middle of the sliding window
            dev.win_time
                = frame.wc_sample_time(ns) + config_.frames_to_ns(dev.win_size / 2);
        }

        dev.win_pos++;

        // once per window, print line
        if (dev.win_pos == dev.win_size) {
            if (!header_printed_) {
                header_printed_ = true;
                print_header_();
            }
            print_line_(dev);
            dev.win_pos = 0;
        }
    }
}

void CsvDumper::print_header_() {
    size_t off = 0;

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "# dir");

    if (config_.show_device_names) {
        off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, ",dev");
    }

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, ",timestamp");

    for (size_t ch = 0; ch < config_.channel_count; ch++) {
        off += (size_t)snprintf(
            buf_.data() + off, buf_.size() - off, ",ch%d", (int)ch + 1);
    }

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "\n");

    fprintf(fp_, "%s", buf_.data());
    fflush(fp_);
}

void CsvDumper::print_line_(const DeviceState& dev) {
    size_t off = 0;

    off += (size_t)snprintf(
        buf_.data() + off, buf_.size() - off, "%s", dev.dir == Dir::Output ? "o" : "i");

    if (config_.show_device_names) {
        off += (size_t)snprintf(
            buf_.data() + off, buf_.size() - off, ",%s", dev.name.c_str());
    }

    off += (size_t)snprintf(
        buf_.data() + off, buf_.size() - off, ",%lld", (long long)dev.win_time);

    for (size_t ch = 0; ch < config_.channel_count; ch++) {
        off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, ",%lld",
            (long long)dev.win_avg[ch].get());
    }

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "\n");

    fprintf(fp_, "%s", buf_.data());
    fflush(fp_);
}

} // namespace signal_estimator
