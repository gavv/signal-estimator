/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaWriter.hpp"
#include "AlsaUtils.hpp"
#include "Log.hpp"

namespace signal_estimator {

AlsaWriter::AlsaWriter(const Config& config)
    : config_(config) {
}

AlsaWriter::~AlsaWriter() {
    close();
}

bool AlsaWriter::open(const char *device) {
    log_info("opening alsa writer for device %s", device);

    pcm_ = alsa_open(device, SND_PCM_STREAM_PLAYBACK, config_, &period_size_);
    return pcm_;
}

void AlsaWriter::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

bool AlsaWriter::write(const int16_t *buf, size_t bufsz) {
    int err = snd_pcm_writei(pcm_, buf, bufsz / config_.n_channels);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            log_info("alsa writer: recovered after xrun");
        }
    }

    if (err < 0) {
        log_error("alsa writer: %s", snd_strerror(err));
        return false;
    }

    return true;
}

} // namespace signal_estimator
