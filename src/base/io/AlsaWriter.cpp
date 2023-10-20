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
    const size_t buf_samples = resize_buf_(frame);

    write_buf_(frame);

    snd_pcm_sframes_t err = snd_pcm_writei(pcm_, buf_.data(), buf_samples / buf_chans_);

    frame.set_time();

    if (err < 0) {
        if ((err = snd_pcm_recover(pcm_, (int)err, 1)) == 0) {
            se_log_info("alsa writer: recovered after xrun");
        }
    }

    if (err < 0) {
        se_log_error("alsa writer: {}", snd_strerror((int)err));
        return false;
    }

    return true;
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
