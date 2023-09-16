// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "sndio/AlsaWriter.hpp"
#include "core/Log.hpp"
#include "sndio/AlsaUtils.hpp"

namespace signal_estimator {

AlsaWriter::~AlsaWriter() {
    close();
}

bool AlsaWriter::open(Config& config, const char* device) {
    se_log_info("opening alsa writer for device %s", device);

    pcm_ = alsa_open(device, SND_PCM_STREAM_PLAYBACK, config);
    config_ = config;

    return pcm_;
}

void AlsaWriter::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

bool AlsaWriter::write(Frame& frame) {
    snd_pcm_sframes_t err
        = snd_pcm_writei(pcm_, frame.data(), frame.size() / config_.n_channels);

    frame.set_time();
    frame.set_type(FrameType::Output);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, (int)err, 1)) == 0) {
            se_log_info("alsa writer: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa writer: %s", snd_strerror((int)err));
        return false;
    }

    return true;
}

} // namespace signal_estimator
