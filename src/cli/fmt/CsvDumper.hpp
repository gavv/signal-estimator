// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"
#include "processing/MovAvg.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace signal_estimator {

// dump samples with their timestamps to a csv file
// to reduce file size, dumps only one maximum value among every N samples
class CsvDumper {
public:
    CsvDumper(const Config& config);
    ~CsvDumper();

    CsvDumper(const CsvDumper&) = delete;
    CsvDumper& operator=(const CsvDumper&) = delete;

    bool open(const char* filename);
    void close();

    void write(Frame& frame);

private:
    void print_line_(IOType type, nanoseconds_t timestamp);

    const Config config_;

    std::vector<MovAvg<double>> mavg_;
    uint64_t pos_ { 0 };

    FILE* fp_ {};
    std::vector<char> buf_;
};

} // namespace signal_estimator
