// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/AlsaUtils.hpp"
#include "core/Log.hpp"

#include <algorithm>
#include <vector>

namespace signal_estimator {

namespace {

std::vector<PcmFormat> alsa_preferred_formats(PcmFormat requested_format) {
    std::vector<PcmFormat> all_formats = PcmFormat::supported_formats();

    // sort by bits() and then by bytes() in ascending order
    std::sort(all_formats.begin(), all_formats.end(),
        [](const PcmFormat& a, const PcmFormat& b) {
            return a.bits() < b.bits() || (a.bits() == b.bits() && a.bytes() < b.bytes());
        });

    std::vector<PcmFormat> ret_fotmats;

    // first try requested format
    ret_fotmats.push_back(requested_format);

    // then try all formats larged than 16 bits in ascending order
    for (const PcmFormat& fmt : all_formats) {
        if (std::find(ret_fotmats.begin(), ret_fotmats.end(), fmt) != ret_fotmats.end()) {
            continue;
        }
        if (fmt.bits() >= 16) {
            ret_fotmats.push_back(fmt);
        }
    }

    // then try the rest
    for (const PcmFormat& fmt : all_formats) {
        if (std::find(ret_fotmats.begin(), ret_fotmats.end(), fmt) != ret_fotmats.end()) {
            continue;
        }
        ret_fotmats.push_back(fmt);
    }

    return ret_fotmats;
}

unsigned int alsa_nearest_buffer_size(
    unsigned int sample_rate, unsigned int latency_us, size_t periods) {
    unsigned int latency_samples
        = (unsigned int)((double)latency_us * sample_rate / 1000000);
    while ((latency_samples % periods) != 0) {
        latency_samples++;
    }
    return latency_samples;
}

bool alsa_set_hw_params(snd_pcm_t* pcm, snd_pcm_access_t access, PcmFormat* sample_format,
    unsigned int sample_rate, unsigned int* channel_count,
    unsigned int requested_latency_us, unsigned int requested_period_count,
    snd_pcm_uframes_t* selected_buffer_size, snd_pcm_uframes_t* selected_period_size) {
    int err = 0;

    snd_pcm_hw_params_t* hw_params = nullptr;
    snd_pcm_hw_params_alloca(&hw_params);

    // initialize hw_params
    if ((err = snd_pcm_hw_params_any(pcm, hw_params)) < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_any(): {}", snd_strerror(err));
        return false;
    }

    // set access
    if ((err = snd_pcm_hw_params_set_access(pcm, hw_params, access)) < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_set_access(): {}", snd_strerror(err));
        return false;
    }

    // set format
    for (const auto fmt : alsa_preferred_formats(*sample_format)) {
        if ((err = snd_pcm_hw_params_set_format(
                 pcm, hw_params, (snd_pcm_format_t)fmt.to_alsa()))
            == 0) {
            *sample_format = fmt;
            break;
        }
    }
    if (err < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_set_format(): {}", snd_strerror(err));
        return false;
    }

    // disable software resampling
    if ((err = snd_pcm_hw_params_set_rate_resample(pcm, hw_params, 0)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_rate_resample(): {}",
            snd_strerror(err));
        return false;
    }

    // set sample rate
    if ((err = snd_pcm_hw_params_set_rate(pcm, hw_params, sample_rate, 0)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_rate_near(): {}",
            snd_strerror(err));
        return false;
    }

    // set channel count
    if ((err = snd_pcm_hw_params_set_channels_near(pcm, hw_params, channel_count)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_channels(): {}",
            snd_strerror(err));
        return false;
    }

    // calculate nearest buffer size that is multiple of # of periods
    unsigned int requested_buffer_size = alsa_nearest_buffer_size(
        sample_rate, requested_latency_us, requested_period_count);

    // set period size in samples
    // ALSA reads 'period_size' samples from circular buffer every period
    *selected_period_size = requested_buffer_size / requested_period_count;
    if ((err = snd_pcm_hw_params_set_period_size_near(
             pcm, hw_params, selected_period_size, nullptr))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_period_size_near(): {}",
            snd_strerror(err));
        return false;
    }

    // get period time
    unsigned int selected_period_time = 0;
    if ((err = snd_pcm_hw_params_get_period_time(
             hw_params, &selected_period_time, nullptr))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_get_period_time(): {}",
            snd_strerror(err));
        return false;
    }

    // set buffer size, i.e. number of samples in circular buffer
    *selected_buffer_size = *selected_period_size * requested_period_count;
    if ((err = snd_pcm_hw_params_set_buffer_size_near(
             pcm, hw_params, selected_buffer_size))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_buffer_size_near(): {}",
            snd_strerror(err));
        return false;
    }

    // get buffer time, i.e. total duration of circular buffer in microseconds,
    // calculated from 'sample_rate' and 'buffer_size'
    unsigned int selected_buffer_time = 0;
    if ((err = snd_pcm_hw_params_get_buffer_time(
             hw_params, &selected_buffer_time, nullptr))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_get_buffer_time(): {}",
            snd_strerror(err));
        return false;
    }

    se_log_debug(
        "buffer_size: {} us ({} samples)", selected_buffer_time, *selected_buffer_size);
    se_log_debug(
        "period_size: {} us ({} samples)", selected_period_time, *selected_period_size);
    se_log_debug("sample_rate: {} Hz", sample_rate);
    se_log_debug("sample_format: {}", sample_format->to_string());
    se_log_debug("channel_count: {}", *channel_count);

    // send hw_params to ALSA
    if ((err = snd_pcm_hw_params(pcm, hw_params)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params(): {}", snd_strerror(err));
        return false;
    }

    return true;
}

bool alsa_set_sw_params(snd_pcm_t* pcm, snd_pcm_stream_t mode,
    snd_pcm_uframes_t period_size, snd_pcm_uframes_t buffer_size) {
    int err = 0;

    snd_pcm_sw_params_t* sw_params = nullptr;
    snd_pcm_sw_params_alloca(&sw_params);

    // initialize sw_params
    if ((err = snd_pcm_sw_params_current(pcm, sw_params)) < 0) {
        se_log_error(
            "can't set sw params: snd_pcm_sw_params_current(): {}", snd_strerror(err));
        return false;
    }

    if (mode == SND_PCM_STREAM_PLAYBACK) {
        // set start threshold, after which ALSA starts playback
        if ((err = snd_pcm_sw_params_set_start_threshold(pcm, sw_params, buffer_size))
            < 0) {
            se_log_error(
                "can't set sw params: snd_pcm_sw_params_set_start_threshold(): {}",
                snd_strerror(err));
            return false;
        }

        // set minimum threshold, below which ALSA won't allow to perform write
        if ((err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, period_size)) < 0) {
            se_log_error("can't set sw params: snd_pcm_sw_params_set_avail_min(): {}",
                snd_strerror(err));
            return false;
        }
    } else {
        // set minimum threshold, below which ALSA won't allow to perform read
        if ((err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, period_size)) < 0) {
            se_log_error("can't set sw params: snd_pcm_sw_params_set_avail_min(): {}",
                snd_strerror(err));
            return false;
        }
    }

    // send sw_params to ALSA
    if ((err = snd_pcm_sw_params(pcm, sw_params)) < 0) {
        se_log_error("can't set sw params: snd_pcm_sw_params(): {}", snd_strerror(err));
        return false;
    }

    return true;
}

} // namespace

snd_pcm_t* alsa_open(const std::string& device, snd_pcm_stream_t stream,
    const Config& config, DevInfo& dev_info) {
    snd_pcm_t* pcm = nullptr;

    int err = 0;
    if ((err = snd_pcm_open(&pcm, device.c_str(), stream, 0)) < 0) {
        se_log_error("can't open alsa device: snd_pcm_open(): {}", snd_strerror(err));
        return nullptr;
    }

    PcmFormat sample_format = stream == SND_PCM_STREAM_PLAYBACK
        ? config.requested_output_format
        : config.requested_input_format;

    unsigned int sample_rate = config.sample_rate;
    unsigned int channel_count = config.channel_count;

    const unsigned int requested_latency_us = stream == SND_PCM_STREAM_PLAYBACK
        ? config.requested_output_latency_us
        : config.requested_input_latency_us;

    const unsigned int requested_period_count = stream == SND_PCM_STREAM_PLAYBACK
        ? config.requested_output_period_count
        : config.requested_input_period_count;

    snd_pcm_uframes_t selected_buffer_size = 0, selected_period_size = 0;

    if (!alsa_set_hw_params(pcm, SND_PCM_ACCESS_RW_INTERLEAVED, &sample_format,
            sample_rate, &channel_count, requested_latency_us, requested_period_count,
            &selected_buffer_size, &selected_period_size)) {
        goto error;
    }

    if (!alsa_set_sw_params(pcm, stream, selected_period_size, selected_buffer_size)) {
        goto error;
    }

    dev_info.dir = stream == SND_PCM_STREAM_PLAYBACK ? Dir::Output : Dir::Input;
    dev_info.short_name = device;
    dev_info.period_count = selected_buffer_size / selected_period_size;
    dev_info.period_size = selected_period_size * config.channel_count;
    dev_info.sample_format = sample_format;
    dev_info.channel_count = channel_count;

    return pcm;

error:
    if ((err = snd_pcm_close(pcm)) < 0) {
        se_log_error("can't close alsa device: snd_pcm_close(): {}", snd_strerror(err));
    }

    return nullptr;
}

void alsa_close(snd_pcm_t* pcm) {
    if (!pcm) {
        return;
    }

    int err = 0;

    if ((err = snd_pcm_close(pcm)) < 0) {
        se_log_warn("can't close alsa device: snd_pcm_close(): {}", snd_strerror(err));
    }
}

} // namespace signal_estimator
