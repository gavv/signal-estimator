// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaReader.hpp"
#include "core/Log.hpp"
#include "io/AlsaUtils.hpp"

namespace signal_estimator {

AlsaReader::~AlsaReader() {
    close();
}

bool AlsaReader::open(Config& config, const char* device) {
    se_log_info("opening alsa reader for device {}", device);

    pcm_ = alsa_open(device, SND_PCM_STREAM_CAPTURE, config);
    config_ = config;

    return pcm_;
}

void AlsaReader::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

bool AlsaReader::read(Frame& frame) {
    snd_pcm_sframes_t err
        = snd_pcm_readi(pcm_, frame.data(), frame.size() / config_.channel_count);

    frame.set_time();

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, (int)err, 1)) == 0) {
            se_log_info("alsa reader: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa reader: {}", snd_strerror((int)err));
        return false;
    }

    return true;
}

} // namespace signal_estimator
