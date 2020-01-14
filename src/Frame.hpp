/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "Time.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace signal_estimator {

using sample_t = int16_t;

constexpr sample_t MaxSample = +32767;
constexpr sample_t MinSample = -32767;

enum class FrameType {
    Output,
    Input,
};

class Frame {
public:
    Frame(const Config&);

    sample_t* data();
    size_t size();

    void set_type(FrameType);
    void set_timestamp();

    // calculate time offset in nanoseconds when the sample will be
    // actually played or was actually recorded
    nanoseconds_t sample_time(size_t offset) const;

private:
    const Config& config_;

    FrameType type_ { FrameType::Output };
    nanoseconds_t timestamp_ {};

    std::vector<sample_t> data_;
};

} // namespace signal_estimator
