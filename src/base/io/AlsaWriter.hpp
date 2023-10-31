// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"
#include "core/Frame.hpp"
#include "io/IDeviceWriter.hpp"
#include "io/PcmMapper.hpp"

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <alsa/asoundlib.h>

namespace signal_estimator {

class AlsaWriter : public IDeviceWriter {
public:
    AlsaWriter() = default;
    ~AlsaWriter() override;

    AlsaWriter(const AlsaWriter&) = delete;
    AlsaWriter& operator=(const AlsaWriter&) = delete;

    bool open(const Config& config, const std::string& device);
    void close();

    DevInfo info() const;

    bool write(Frame& frame) override;

private:
    int write_(Frame& frame);

    Config config_;
    DevInfo dev_info_;

    snd_pcm_t* pcm_ {};

    std::unique_ptr<PcmMapper> mapper_;
    std::vector<uint8_t> map_buf_;

    size_t dev_chans_ {};
    size_t frame_chans_ {};
};

} // namespace signal_estimator
