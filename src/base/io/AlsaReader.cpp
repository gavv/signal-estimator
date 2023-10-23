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

    frame_chans_ = config.channel_count;
    buf_chans_ = config.input_channel_count;

    buf_.resize(config.input_period_size / frame_chans_ * buf_chans_);

    return pcm_;
}

void AlsaReader::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
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
    const size_t buf_samples = resize_buf_(frame);

    // read from device to buffer
    if (snd_pcm_sframes_t err
        = snd_pcm_readi(pcm_, buf_.data(), buf_samples / buf_chans_);
        err < 0) {
        return (int)err;
    }

    // number of samples in ring buffer available for read
    snd_pcm_sframes_t avail = 0;
    // when next sample read from ring buffer was heard
    // depending on driver, this may or may not take into account ADC delay
    snd_pcm_sframes_t delay = 0;
    if (int err = snd_pcm_avail_delay(pcm_, &avail, &delay); err < 0) {
        return err;
    }

    const nanoseconds_t sw_time = monotonic_timestamp_ns();
    const nanoseconds_t hw_time = sw_time - config_.frames_to_ns((size_t)delay)
        + config_.frames_to_ns(buf_samples / buf_chans_);
    const nanoseconds_t hw_buf = config_.frames_to_ns((size_t)avail);

    frame.set_times(sw_time, hw_time, hw_buf);

    // read from buffer to frame
    read_buf_(frame);

    return 0;
}

size_t AlsaReader::resize_buf_(const Frame& frame) {
    const size_t n_samples = frame.size() / frame_chans_ * buf_chans_;

    if (buf_.size() < n_samples) {
        buf_.resize(n_samples);
    }

    return n_samples;
}

void AlsaReader::read_buf_(Frame& frame) {
    sample_t* frame_ptr = frame.data();
    const sample_t* buf_ptr = buf_.data();

    for (size_t ns = 0; ns < frame.size() / frame_chans_; ns++) {
        size_t frame_pos = 0, buf_pos = 0;

        while (frame_pos < frame_chans_ && buf_pos < buf_chans_) {
            frame_ptr[frame_pos++] = buf_ptr[buf_pos++];
        }

        while (frame_pos < frame_chans_) {
            frame_ptr[frame_pos++] = buf_ptr[buf_chans_ - 1];
        }

        frame_ptr += frame_chans_;
        buf_ptr += buf_chans_;
    }
}

} // namespace signal_estimator
