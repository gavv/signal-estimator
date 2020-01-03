/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaReader.hpp"
#include "AlsaUtils.hpp"
#include "Log.hpp"

namespace signal_estimator {

AlsaReader::AlsaReader(const Config& config)
    : config_(config) {
}

AlsaReader::~AlsaReader() {
    close();
}

bool AlsaReader::open(const char *device) {
    log_info("opening alsa reader for device %s", device);

    pcm_ = alsa_open(device, SND_PCM_STREAM_CAPTURE, config_, &period_size_);
    return pcm_;
}

void AlsaReader::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

bool AlsaReader::read(int16_t *buf, size_t bufsz) {
    int err = snd_pcm_readi(pcm_, buf, bufsz / config_.n_channels);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            log_info("alsa reader: recovered after xrun");
        }
    }

    if (err < 0) {
        log_error("alsa reader: %s", snd_strerror(err));
        return false;
    }

    return true;
}

} // namespace signal_estimator
