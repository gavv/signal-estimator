/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaReader.hpp"
#include "AlsaUtils.hpp"
#include "Log.hpp"

namespace signal_estimator {

AlsaReader::~AlsaReader() {
    close();
}

bool AlsaReader::open(Config& config, const char *device) {
    se_log_info("opening alsa reader for device %s", device);

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
    int err = snd_pcm_readi(pcm_, frame.data(), frame.size() / config_.n_channels);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            se_log_info("alsa reader: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa reader: %s", snd_strerror(err));
        return false;
    }

    frame.set_timestamp();
    frame.set_type(FrameType::Input);

    return true;
}

} // namespace signal_estimator
