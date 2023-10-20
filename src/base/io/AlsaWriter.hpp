// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"
#include "io/IDeviceWriter.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaWriter : public IDeviceWriter {
public:
    AlsaWriter() = default;
    ~AlsaWriter() override;

    AlsaWriter(const AlsaWriter&) = delete;
    AlsaWriter& operator=(const AlsaWriter&) = delete;

    bool open(Config& config, const char* device);
    void close();

    bool write(Frame& frame) override;

private:
    int write_(Frame& frame);

    size_t resize_buf_(const Frame& frame);
    void write_buf_(const Frame& frame);

    Config config_;
    snd_pcm_t* pcm_ {};

    std::vector<sample_t> buf_;
    size_t buf_chans_ {};
    size_t frame_chans_ {};
};

} // namespace signal_estimator
