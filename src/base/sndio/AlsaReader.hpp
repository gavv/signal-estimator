// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"
#include "sndio/IDeviceReader.hpp"

#include <cstdint>
#include <cstdlib>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaReader : public IDeviceReader {
public:
    AlsaReader() = default;
    ~AlsaReader() override;

    AlsaReader(const AlsaReader&) = delete;
    AlsaReader& operator=(const AlsaReader&) = delete;

    bool open(Config& config, const char* device);
    void close();

    bool read(Frame& frame) override;

private:
    Config config_;
    snd_pcm_t* pcm_ {};
};

} // namespace signal_estimator
