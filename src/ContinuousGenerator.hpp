/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "IGenerator.hpp"

namespace signal_estimator {

class ContinuousGenerator : public IGenerator {
public:
    ContinuousGenerator(const Config& config);

    ContinuousGenerator(const ContinuousGenerator&) = delete;
    ContinuousGenerator& operator=(const ContinuousGenerator&) = delete;

    void generate(int16_t* buf, size_t bufsz) override;

private:
    const Config config_;
    size_t pos_ {};
};

} // namespace signal_estimator
