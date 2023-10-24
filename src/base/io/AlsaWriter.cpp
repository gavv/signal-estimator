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
    dev_buf_.resize(dev_info_.period_size / frame_chans_ * dev_chans_);

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
    // ensure that buffer size is fine
    const size_t dev_samples = resize_buf_(frame);

    // write from frame to buffer
    write_buf_(frame);

    // write from buffer to device
    if (snd_pcm_sframes_t err
        = snd_pcm_writei(pcm_, dev_buf_.data(), dev_samples / dev_chans_);
        err < 0) {
        return (int)err;
    }

    // number of samples in ring buffer available for write
    snd_pcm_sframes_t avail = 0;
    // when next sample written to ring buffer will be audible
    // depending on driver, this may or may not take into account DAC delay
    snd_pcm_sframes_t delay = 0;
    if (int err = snd_pcm_avail_delay(pcm_, &avail, &delay); err < 0) {
        return err;
    }

    const nanoseconds_t sw_time = monotonic_timestamp_ns();
    const nanoseconds_t hw_time = sw_time + config_.frames_to_ns((size_t)delay)
        - config_.frames_to_ns(dev_samples / dev_chans_);
    const nanoseconds_t hw_buf = config_.frames_to_ns(
        dev_info_.period_count * dev_info_.period_size / frame_chans_ - (size_t)avail);

    frame.set_times(sw_time, hw_time, hw_buf);

    return 0;
}

size_t AlsaWriter::resize_buf_(const Frame& frame) {
    const size_t dev_samples = frame.size() / frame_chans_ * dev_chans_;

    if (dev_buf_.size() < dev_samples) {
        dev_buf_.resize(dev_samples);
    }

    return dev_samples;
}

void AlsaWriter::write_buf_(const Frame& frame) {
    sample_t* buf_ptr = dev_buf_.data();
    const sample_t* frame_ptr = frame.data();

    for (size_t ns = 0; ns < frame.size() / frame_chans_; ns++) {
        size_t buf_pos = 0, frame_pos = 0;

        while (buf_pos < dev_chans_ && frame_pos < frame_chans_) {
            buf_ptr[buf_pos++] = frame_ptr[frame_pos++];
        }

        while (buf_pos < dev_chans_) {
            buf_ptr[buf_pos++] = frame_ptr[frame_chans_ - 1];
        }

        buf_ptr += dev_chans_;
        frame_ptr += frame_chans_;
    }
}

} // namespace signal_estimator
