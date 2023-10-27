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

CsvDumper::CsvDumper(const Config& config)
    : config_(config) {
    win_size_ = std::max(config.dump_compression, size_t(1));

    for (size_t ch = 0; ch < config_.channel_count; ch++) {
        win_avg_.emplace_back(win_size_);
    }

    // upper bound
    buf_.resize(128 + 24 * config_.channel_count);
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

    if (!header_printed_) {
        header_printed_ = true;
        print_header_();
    }

    print_frame_(*frame);
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

void CsvDumper::print_frame_(const Frame& frame) {
    for (size_t n = 0; n < frame.size(); n++) {
        if (n % config_.channel_count == 0) {
            if (win_pos_ == win_size_) {
                print_line_(frame.dir(), quote_dev_(frame.dev()), *win_time_);

                win_time_ = {};
                win_pos_ = 0;
            }
            if (!win_time_) {
                win_time_ = frame.wc_sample_time(n)
                    + config_.samples_to_ns(win_size_ / 2 * config_.channel_count);
            }
            win_pos_++;
        }
        win_avg_[n % config_.channel_count].add(frame[n]);
    }
}

void CsvDumper::print_line_(Dir dir, const char* dev, nanoseconds_t timestamp) {
    size_t off = 0;

    off += (size_t)snprintf(
        buf_.data() + off, buf_.size() - off, "%s", dir == Dir::Output ? "o" : "i");

    if (config_.show_device_names) {
        off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, ",%s", dev);
    }

    off += (size_t)snprintf(
        buf_.data() + off, buf_.size() - off, ",%lld", (long long)timestamp);

    for (size_t ch = 0; ch < config_.channel_count; ch++) {
        off += (size_t)snprintf(
            buf_.data() + off, buf_.size() - off, ",%lld", (long long)win_avg_[ch].get());
    }

    off += (size_t)snprintf(buf_.data() + off, buf_.size() - off, "\n");

    fprintf(fp_, "%s", buf_.data());
    fflush(fp_);
}

const char* CsvDumper::quote_dev_(const DevInfo& dev_info) {
    if (!config_.show_device_names) {
        return nullptr;
    }

    if (!quoted_devs_.count(dev_info.short_name)) {
        std::string quoted_name = dev_info.short_name;

        if (quoted_name.find(',') != std::string::npos
            || quoted_name.find('"') != std::string::npos
            || quoted_name.find('\n') != std::string::npos
            || quoted_name.find('\r') != std::string::npos) {
            quoted_name = std::regex_replace(quoted_name, std::regex("\""), "\"\"");
            quoted_name = "\"" + quoted_name + "\"";
        }

        quoted_devs_[dev_info.short_name] = std::move(quoted_name);
    }

    return quoted_devs_.at(dev_info.short_name).c_str();
}

} // namespace signal_estimator
