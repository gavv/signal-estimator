// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"

#include <alsa/asoundlib.h>

namespace signal_estimator {

// open and setup ALSA PCM device
snd_pcm_t* alsa_open(const char* device, snd_pcm_stream_t mode, Config& config);

//set card volume to max
void alsa_vol_set(const char *device, long volume);

// close ALSA PCM device
void alsa_close(snd_pcm_t* pcm);

} // namespace signal_estimator
