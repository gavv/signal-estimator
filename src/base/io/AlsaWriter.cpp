// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaWriter.hpp"
#include "core/Log.hpp"
#include "io/AlsaUtils.hpp"

namespace signal_estimator {

AlsaWriter::~AlsaWriter() {
    close();
}

bool AlsaWriter::open(const Config& config, const std::string& device) {
    se_log_info("opening alsa writer for device {}", device);

    config_ = config;

    pcm_ = alsa_open(device.c_str(), SND_PCM_STREAM_PLAYBACK, config_, dev_info_);

    frame_chans_ = config_.channel_count;
    dev_chans_ = dev_info_.channel_count;

    mapper_ = std::make_unique<PcmMapper>(
        /* input:  */ PcmFormat(), config.channel_count,
        /* output: */ dev_info_.sample_format, dev_info_.channel_count);

    map_buf_.resize(
        mapper_->output_bytes(dev_info_.period_size / frame_chans_ * dev_chans_));

    return pcm_;
}

void AlsaWriter::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
}

DevInfo AlsaWriter::info() const {
    return dev_info_;
}

bool AlsaWriter::write(Frame& frame) {
    int err = write_(frame);

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, err, 1)) == 0) {
            se_log_info("alsa writer: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa writer: {}", snd_strerror((int)err));
        return false;
    }

    return true;
}

int AlsaWriter::write_(Frame& frame) {
    const size_t samples_per_chan = frame.size() / frame_chans_;

    // prepare buffer
    if (const size_t n_bytes = mapper_->output_bytes(samples_per_chan);
        map_buf_.size() < n_bytes) {
        map_buf_.resize(n_bytes);
    }

    // write from frame to buffer
    mapper_->map((const uint8_t*)frame.data(), map_buf_.data(), samples_per_chan);

    // write from buffer to device
    if (snd_pcm_sframes_t err = snd_pcm_writei(pcm_, map_buf_.data(), samples_per_chan);
        err < 0) {
        return (int)err;
    }

    // avail: free space in ring buffer
    // delay: delay from ring buffer to DAC
    snd_pcm_sframes_t avail = 0, delay = 0;
    if (int err = snd_pcm_avail_delay(pcm_, &avail, &delay); err < 0) {
        return err;
    }

    // queued: number of samples in ring buffer
    const size_t queued
        = dev_info_.period_count * dev_info_.period_size / frame_chans_ - (size_t)avail;

    const nanoseconds_t sw_time = monotonic_timestamp_ns();
    const nanoseconds_t hw_time
        = sw_time + config_.frames_to_ns(queued) - config_.frames_to_ns(samples_per_chan);
    const nanoseconds_t wc_time = wallclock_timestamp_ns() + config_.frames_to_ns(queued)
        - config_.frames_to_ns(samples_per_chan);

    const nanoseconds_t sw_delay = config_.frames_to_ns((size_t)queued);
    const nanoseconds_t hw_delay = config_.frames_to_ns((size_t)delay) - sw_delay;

    frame.set_times(sw_time, hw_time, wc_time, sw_delay, hw_delay);

    return 0;
}

} // namespace signal_estimator
