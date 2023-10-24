// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"

#include <alsa/asoundlib.h>

#include <string>

namespace signal_estimator {

// open and setup ALSA PCM device
snd_pcm_t* alsa_open(const std::string& device, snd_pcm_stream_t stream,
    const Config& config, DevInfo& dev_info);

// close ALSA PCM device
void alsa_close(snd_pcm_t* pcm);

} // namespace signal_estimator
