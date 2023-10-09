// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "sndio/AlsaUtils.hpp"
#include "core/Log.hpp"

namespace signal_estimator {

namespace {

unsigned int alsa_nearest_buffer_size(
    unsigned int sample_rate, int n_channels, unsigned int latency_us, size_t periods) {
    unsigned int latency_samples
        = (unsigned int)((double)latency_us * sample_rate / 1000000);
    while ((latency_samples * (unsigned int)n_channels) % periods != 0) {
        latency_samples++;
    }
    return latency_samples;
}

bool alsa_set_hw_params(snd_pcm_t* pcm, snd_pcm_uframes_t* period_size,
    snd_pcm_uframes_t* buffer_size, snd_pcm_access_t access, snd_pcm_format_t format,
    unsigned int sample_rate, int n_channels, unsigned int n_periods,
    unsigned int latency_us) {
    int err = 0;

    snd_pcm_hw_params_t* hw_params = nullptr;
    snd_pcm_hw_params_alloca(&hw_params);

    // initialize hw_params
    if ((err = snd_pcm_hw_params_any(pcm, hw_params)) < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_any(): %s", snd_strerror(err));
        return false;
    }

    // disable software resampling
    if ((err = snd_pcm_hw_params_set_rate_resample(pcm, hw_params, 0)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_rate_resample: %s",
            snd_strerror(err));
        return false;
    }

    // set number of channels
    if ((err = snd_pcm_hw_params_set_channels(pcm, hw_params, (unsigned int)n_channels))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_channels(): %s",
            snd_strerror(err));
        return false;
    }

    // set access
    if ((err = snd_pcm_hw_params_set_access(pcm, hw_params, access)) < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_set_access(): %s", snd_strerror(err));
        return false;
    }

    // set format
    if ((err = snd_pcm_hw_params_set_format(pcm, hw_params, format)) < 0) {
        se_log_error(
            "can't set hw params: snd_pcm_hw_params_set_format(): %s", snd_strerror(err));
        return false;
    }

    // set sample rate
    unsigned int rate = sample_rate;
    if ((err = snd_pcm_hw_params_set_rate_near(pcm, hw_params, &rate, nullptr)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_rate_near(): %s",
            snd_strerror(err));
        return false;
    }
    if (rate != sample_rate) {
        se_log_error("can't set hw params: exact sample rate value is not supported:"
                     " requested=%d supported=%d",
            (int)sample_rate, (int)rate);
        return false;
    }

    // calculate nearest buffer size that is multiple of # of periods
    unsigned int suggested_buffer_size
        = alsa_nearest_buffer_size(sample_rate, n_channels, latency_us, n_periods);

    // set period size in samples
    // ALSA reads 'period_size' samples from circular buffer every period
    *period_size = suggested_buffer_size / n_periods;
    if ((err = snd_pcm_hw_params_set_period_size_near(
             pcm, hw_params, period_size, nullptr))
        < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_period_size_near(): %s",
            snd_strerror(err));
        return false;
    }

    // get period time
    unsigned int period_time = 0;
    if ((err = snd_pcm_hw_params_get_period_time(hw_params, &period_time, nullptr)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_get_period_time(): %s",
            snd_strerror(err));
        return false;
    }

    // set buffer size, i.e. number of samples in circular buffer
    *buffer_size = *period_size * n_periods;
    if ((err = snd_pcm_hw_params_set_buffer_size_near(pcm, hw_params, buffer_size)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_set_buffer_size_near(): %s",
            snd_strerror(err));
        return false;
    }

    // get buffer time, i.e. total duration of circular buffer in microseconds,
    // calculated from 'sample_rate' and 'buffer_size'
    unsigned int buffer_time = 0;
    if ((err = snd_pcm_hw_params_get_buffer_time(hw_params, &buffer_time, nullptr)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params_get_buffer_time(): %s",
            snd_strerror(err));
        return false;
    }

    se_log_info("suggested_latency: %u us", (unsigned)latency_us);
    se_log_info("suggested_buffer_size: %u samples", (unsigned)suggested_buffer_size);
    se_log_info("selected_buffer_time: %u us", (unsigned)buffer_time);
    se_log_info("selected_buffer_size: %u samples", (unsigned)*buffer_size);
    se_log_info("selected_period_time: %u us", (unsigned)period_time);
    se_log_info("selected_period_size: %u samples", (unsigned)*period_size);

    // send hw_params to ALSA
    if ((err = snd_pcm_hw_params(pcm, hw_params)) < 0) {
        se_log_error("can't set hw params: snd_pcm_hw_params(): %s", snd_strerror(err));
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
            "can't set sw params: snd_pcm_sw_params_current(): %s", snd_strerror(err));
        return false;
    }

    if (mode == SND_PCM_STREAM_PLAYBACK) {
        // set start threshold, after which ALSA starts playback
        if ((err = snd_pcm_sw_params_set_start_threshold(pcm, sw_params, buffer_size))
            < 0) {
            se_log_error(
                "can't set sw params: snd_pcm_sw_params_set_start_threshold(): %s",
                snd_strerror(err));
            return false;
        }

        // set minimum threshold, below which ALSA wont allow to perform write
        if ((err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, period_size)) < 0) {
            se_log_error("can't set sw params: snd_pcm_sw_params_set_avail_min(): %s",
                snd_strerror(err));
            return false;
        }
    } else {
        // set minimum threshold, below which ALSA wont allow to perform read
        if ((err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, period_size)) < 0) {
            se_log_error("can't set sw params: snd_pcm_sw_params_set_avail_min(): %s",
                snd_strerror(err));
            return false;
        }
    }

    // send sw_params to ALSA
    if ((err = snd_pcm_sw_params(pcm, sw_params)) < 0) {
        se_log_error("can't set sw params: snd_pcm_sw_params(): %s", snd_strerror(err));
        return false;
    }

    return true;
}

} // namespace

void alsa_vol_set(const char *device, long volume) {
    snd_mixer_t *mixer;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t* elem;
    long min, max;
    int err;
    
    if ((err = snd_mixer_open(&mixer, 0)) < 0) {
        se_log_error("can't open alsa mixer: snd_mixer_open(): %s", snd_strerror(err));
        return;
    }
    
    if (snd_mixer_attach(mixer, device))
        goto close_mixer;
    
    if (snd_mixer_selem_register(mixer, nullptr, nullptr))
        goto close_mixer;
    
    if (snd_mixer_load(mixer))
        goto close_mixer;
    
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    if (!(elem = snd_mixer_find_selem(mixer, sid)))
        goto close_mixer;

    if(snd_mixer_selem_get_playback_volume_range(elem, &min, &max))
        goto close_mixer;

    if (snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100))
        goto close_mixer;
    

close_mixer:
    if ((err = snd_mixer_close(mixer)) < 0) {
        se_log_error("can't close alsa mixer: snd_mixer_close(): %s", snd_strerror(err));
        return;
    }

}


snd_pcm_t* alsa_open(const char* device, snd_pcm_stream_t mode, Config& config) {
    snd_pcm_t* pcm = nullptr;

    int err = 0;
    if ((err = snd_pcm_open(&pcm, device, mode, 0)) < 0) {
        se_log_error("can't open alsa device: snd_pcm_open(): %s", snd_strerror(err));
        return nullptr;
    }

    snd_pcm_uframes_t period_size = 0, buffer_size = 0;

    if (!alsa_set_hw_params(pcm, &period_size, &buffer_size,
            SND_PCM_ACCESS_RW_INTERLEAVED, SND_PCM_FORMAT_S16_LE, config.sample_rate,
            (int)config.n_channels, config.io_num_periods, config.io_latency_us)) {
        goto error;
    }

    if (!alsa_set_sw_params(pcm, mode, period_size, buffer_size)) {
        goto error;
    }

    config.io_period_size = size_t(period_size * config.n_channels);

    return pcm;

error:
    if ((err = snd_pcm_close(pcm)) < 0) {
        se_log_error("can't close alsa device: snd_pcm_close(): %s", snd_strerror(err));
    }

    return nullptr;
}

void alsa_close(snd_pcm_t* pcm) {
    if (!pcm) {
        return;
    }

    int err = 0;

    if ((err = snd_pcm_close(pcm)) < 0) {
        se_log_error("can't close alsa device: snd_pcm_close(): %s", snd_strerror(err));
    }
}

} // namespace signal_estimator
