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
    dev_buf_.resize(dev_info_.period_size / frame_chans_ * dev_chans_);

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
    // ensure that buffer size is fine
    const size_t dev_samples = resize_buf_(frame);

    // read from device to buffer
    if (snd_pcm_sframes_t err
        = snd_pcm_readi(pcm_, dev_buf_.data(), dev_samples / dev_chans_);
        err < 0) {
        return (int)err;
    }

    // number of samples in ring buffer available for read
    const snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_);

    nanoseconds_t sw_time = monotonic_timestamp_ns();
    nanoseconds_t hw_time = sw_time - config_.frames_to_ns((size_t)avail)
        - config_.frames_to_ns(dev_samples / dev_chans_);
    nanoseconds_t wc_time = wallclock_timestamp_ns() - config_.frames_to_ns((size_t)avail)
        - config_.frames_to_ns(dev_samples / dev_chans_);
    nanoseconds_t hw_buf = config_.frames_to_ns((size_t)avail);

    frame.set_times(sw_time, hw_time, wc_time, hw_buf);

    // read from buffer to frame
    read_buf_(frame);

    return 0;
}

size_t AlsaReader::resize_buf_(const Frame& frame) {
    const size_t dev_samples = frame.size() / frame_chans_ * dev_chans_;

    if (dev_buf_.size() < dev_samples) {
        dev_buf_.resize(dev_samples);
    }

    return dev_samples;
}

void AlsaReader::read_buf_(Frame& frame) {
    sample_t* frame_ptr = frame.data();
    const sample_t* buf_ptr = dev_buf_.data();

    for (size_t ns = 0; ns < frame.size() / frame_chans_; ns++) {
        size_t frame_pos = 0, buf_pos = 0;

        while (frame_pos < frame_chans_ && buf_pos < dev_chans_) {
            frame_ptr[frame_pos++] = buf_ptr[buf_pos++];
        }

        while (frame_pos < frame_chans_) {
            frame_ptr[frame_pos++] = buf_ptr[dev_chans_ - 1];
        }

        frame_ptr += frame_chans_;
        buf_ptr += dev_chans_;
    }
}

} // namespace signal_estimator
