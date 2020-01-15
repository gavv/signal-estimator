/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "AlsaWriter.hpp"
#include "AlsaUtils.hpp"
#include "Log.hpp"

namespace signal_estimator {

AlsaWriter::~AlsaWriter() {
    close();
}

bool AlsaWriter::open(Config& config, const char *device) {
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
    frame.mark_io_begin(IOType::Output);

    int err = snd_pcm_writei(pcm_, frame.data(), frame.size() / config_.n_channels);

    frame.mark_io_end();

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            se_log_info("alsa writer: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa writer: %s", snd_strerror(err));
        return false;
    }

    return true;
}

} // namespace signal_estimator
