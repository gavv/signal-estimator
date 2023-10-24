// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"
#include "core/Frame.hpp"
#include "io/IDeviceReader.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaReader : public IDeviceReader {
public:
    AlsaReader() = default;
    ~AlsaReader() override;

    AlsaReader(const AlsaReader&) = delete;
    AlsaReader& operator=(const AlsaReader&) = delete;

    bool open(const Config& config, const std::string& device);
    void close();

    DevInfo info() const;

    bool read(Frame& frame) override;

private:
    int read_(Frame& frame);

    size_t resize_buf_(const Frame& frame);
    void read_buf_(Frame& frame);

    Config config_;
    DevInfo dev_info_;
    snd_pcm_t* pcm_ {};

    std::vector<sample_t> dev_buf_;
    size_t dev_chans_ {};
    size_t frame_chans_ {};
};

} // namespace signal_estimator
