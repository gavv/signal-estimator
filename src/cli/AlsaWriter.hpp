/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "Frame.hpp"

#include <cstdint>
#include <cstdlib>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaWriter {
public:
    AlsaWriter() = default;
    ~AlsaWriter();

    AlsaWriter(const AlsaWriter&) = delete;
    AlsaWriter& operator=(const AlsaWriter&) = delete;

    bool open(Config& config, const char* device);
    void close();

    bool write(Frame& frame);

private:
    Config config_;
    snd_pcm_t* pcm_ {};
};

} // namespace signal_estimator
