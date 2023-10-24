// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"
#include "dumps/IDumper.hpp"
#include "processing/MovAvg.hpp"

#include <cstdlib>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace signal_estimator {

// dumps samples with their timestamps to a csv file
// to reduce file size, can "compress" output by replacing each N
// samples with an average value among those samples, where N is
// defined by config.dump_compression
class CsvDumper : public IDumper {
public:
    CsvDumper(const Config& config);
    ~CsvDumper();

    CsvDumper(const CsvDumper&) = delete;
    CsvDumper& operator=(const CsvDumper&) = delete;

    bool open(const std::string& filename);
    void close();

    void write(FramePtr frame) override;

private:
    void print_header_();
    void print_frame_(const Frame& frame);
    void print_line_(Dir dir, const char* dev, nanoseconds_t timestamp);

    const char* quote_dev_(const DevInfo& dev_info);

    const Config config_;

    bool header_printed_ { false };

    std::vector<MovAvg<float>> win_avg_;
    std::optional<nanoseconds_t> win_time_ { 0 };
    size_t win_size_ { 0 };
    size_t win_pos_ { 0 };

    FILE* fp_ {};
    std::vector<char> buf_;

    std::unordered_map<std::string, std::string> quoted_devs_;
};

} // namespace signal_estimator
