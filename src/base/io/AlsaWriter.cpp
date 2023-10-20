// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaWriter.hpp"
#include "core/Log.hpp"
#include "io/AlsaUtils.hpp"

namespace signal_estimator {

AlsaWriter::~AlsaWriter() {
    close();
}

bool AlsaWriter::open(Config& config, const char* device) {
    se_log_info("opening alsa writer for device {}", device);

    pcm_ = alsa_open(device, SND_PCM_STREAM_PLAYBACK, config);
    config_ = config;

    frame_chans_ = config.channel_count;
    buf_chans_ = config.output_channel_count;

    buf_.resize(config.output_period_size / frame_chans_ * buf_chans_);

    return pcm_;
}

void AlsaWriter::close() {
    if (pcm_) {
        alsa_close(pcm_);
        pcm_ = nullptr;
    }
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
    const size_t buf_samples = resize_buf_(frame);

    // write from frame to buffer
    write_buf_(frame);

    // write from buffer to device
    if (snd_pcm_sframes_t err
        = snd_pcm_writei(pcm_, buf_.data(), buf_samples / buf_chans_);
        err < 0) {
        return (int)err;
    }

    // ask when next sample that we will write will become audible
    snd_pcm_sframes_t delay = 0;
    if (int err = snd_pcm_delay(pcm_, &delay); err < 0) {
        return err;
    }

    const nanoseconds_t sw_time = monotonic_timestamp_ns();
    const nanoseconds_t hw_time = sw_time + config_.frames_to_ns(delay)
        - config_.frames_to_ns(buf_samples / buf_chans_);

    frame.set_times(sw_time, hw_time);

    return 0;
}

size_t AlsaWriter::resize_buf_(const Frame& frame) {
    const size_t n_samples = frame.size() / frame_chans_ * buf_chans_;

    if (buf_.size() < n_samples) {
        buf_.resize(n_samples);
    }

    return n_samples;
}

void AlsaWriter::write_buf_(const Frame& frame) {
    sample_t* buf_ptr = buf_.data();
    const sample_t* frame_ptr = frame.data();

    for (size_t ns = 0; ns < frame.size() / frame_chans_; ns++) {
        size_t buf_pos = 0, frame_pos = 0;

        while (buf_pos < buf_chans_ && frame_pos < frame_chans_) {
            buf_ptr[buf_pos++] = frame_ptr[frame_pos++];
        }

        while (buf_pos < buf_chans_) {
            buf_ptr[buf_pos++] = frame_ptr[frame_chans_ - 1];
        }

        buf_ptr += buf_chans_;
        frame_ptr += frame_chans_;
    }
}

} // namespace signal_estimator
