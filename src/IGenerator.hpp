/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class IGenerator {
public:
    virtual ~IGenerator() = default;

    virtual void generate(int16_t* buf, size_t bufsz) = 0;
};

} // namespace signal_estimator
