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

class AlsaReader {
public:
    AlsaReader(const Config& config);
    ~AlsaReader();

    AlsaReader(const AlsaReader&) = delete;
    AlsaReader& operator=(const AlsaReader&) = delete;

    size_t period_size() const { return period_size_; }

    bool open(const char *device);
    void close();

    bool read(int16_t* buf, size_t bufsz);

private:
    const Config config_;
    snd_pcm_t* pcm_ {};
    size_t period_size_ {};
};

} // namespace signal_estimator
