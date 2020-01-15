/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "IGenerator.hpp"

namespace signal_estimator {

// generate continuous sine wave
class ContinuousGenerator : public IGenerator {
public:
    ContinuousGenerator(const Config& config);

    ContinuousGenerator(const ContinuousGenerator&) = delete;
    ContinuousGenerator& operator=(const ContinuousGenerator&) = delete;

    void generate(Frame& frame) override;

private:
    const Config config_;
    size_t pos_ {};
};

} // namespace signal_estimator
