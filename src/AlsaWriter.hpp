/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"

#include <cstdint>
#include <cstdlib>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaWriter {
public:
    AlsaWriter(const Config& config);
    ~AlsaWriter();

    AlsaWriter(const AlsaWriter&) = delete;
    AlsaWriter& operator=(const AlsaWriter&) = delete;

    size_t period_size() const { return period_size_; }

    bool open(const char *device);
    void close();

    bool write(const int16_t* buf, size_t bufsz);

private:
    const Config config_;
    snd_pcm_t* pcm_ {};
    size_t period_size_ {};
};

} // namespace signal_estimator
