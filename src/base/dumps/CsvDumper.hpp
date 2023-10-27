// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"
#include "dumps/IDumper.hpp"
#include "processing/MovAvg.hpp"

#include <cstdlib>
#include <map>
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
    struct DeviceState {
        Dir dir { Dir::Output };
        std::string name;

        std::vector<MovAvg<float>> win_avg;
        size_t win_size { 0 };
        size_t win_pos { 0 };
        nanoseconds_t win_time { 0 };
    };

    void device_init_(DeviceState& dev, const Frame& frame);
    void device_add_(DeviceState& dev, const Frame& frame);

    void print_header_();
    void print_line_(const DeviceState& dev);

    const Config config_;

    std::map<Dir, std::unordered_map<size_t, DeviceState>> devices_;
    bool header_printed_ { false };

    FILE* fp_ {};
    std::vector<char> buf_;
};

} // namespace signal_estimator
