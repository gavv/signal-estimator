/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "IGenerator.hpp"

namespace signal_estimator {

// generate periodic beeps ("strikes")
class StrikeGenerator : public IGenerator {
public:
    StrikeGenerator(const Config& config);

    StrikeGenerator(const StrikeGenerator&) = delete;
    StrikeGenerator& operator=(const StrikeGenerator&) = delete;

    void generate(Frame& frame) override;

private:
    const Config config_;

    const size_t strike_period_;
    const size_t strike_length_;

    size_t warmup_countdown_ {};

    size_t strike_countdown_ {};
    size_t strike_pos_ {};
};

} // namespace signal_estimator
