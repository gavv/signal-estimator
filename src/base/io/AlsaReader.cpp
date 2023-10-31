// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaReader.hpp"
#include "core/Log.hpp"
#include "io/AlsaUtils.hpp"

namespace signal_estimator {

AlsaReader::~AlsaReader() {
    close();
}

bool AlsaReader::open(const Config& config, const std::string& device) {
    se_log_info("opening alsa reader for device {}", device);

    config_ = config;

    pcm_ = alsa_open(device.c_str(), SND_PCM_STREAM_CAPTURE, config_, dev_info_);

    frame_chans_ = config.channel_count;
    dev_chans_ = dev_info_.channel_count;

    mapper_ = std::make_unique<PcmMapper>(
        /* input:  */ dev_info_.sample_format, dev_info_.channel_count,
        /* output: */ PcmFormat(), config.channel_count);

    map_buf_.resize(
        mapper_->input_bytes(dev_info_.period_size / frame_chans_ * dev_chans_));

    return pcm_;
}

void AlsaReader::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

DevInfo AlsaReader::info() const {
    return dev_info_;
}

bool AlsaReader::read(Frame& frame) {
    int err = read_(frame);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            se_log_info("alsa reader: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa reader: {}", snd_strerror((int)err));
        return false;
    }

    return true;
}

int AlsaReader::read_(Frame& frame) {
    const size_t samples_per_chan = frame.size() / frame_chans_;

    // prepare buffer
    if (const size_t n_bytes = mapper_->input_bytes(samples_per_chan);
        map_buf_.size() < n_bytes) {
        map_buf_.resize(n_bytes);
    }

    // read from device to buffer
    if (snd_pcm_sframes_t err = snd_pcm_readi(pcm_, map_buf_.data(), samples_per_chan);
        err < 0) {
        return (int)err;
    }

    // number of samples in ring buffer available for read
    const snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_);

    nanoseconds_t sw_time = monotonic_timestamp_ns();
    nanoseconds_t hw_time = sw_time - config_.frames_to_ns((size_t)avail)
        - config_.frames_to_ns(samples_per_chan);
    nanoseconds_t wc_time = wallclock_timestamp_ns() - config_.frames_to_ns((size_t)avail)
        - config_.frames_to_ns(samples_per_chan);
    nanoseconds_t hw_buf = config_.frames_to_ns((size_t)avail);

    frame.set_times(sw_time, hw_time, wc_time, hw_buf);

    // read from buffer to frame
    mapper_->map(map_buf_.data(), (uint8_t*)frame.data(), samples_per_chan);

    return 0;
}

} // namespace signal_estimator
