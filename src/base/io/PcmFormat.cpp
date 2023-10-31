// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/PcmFormat.hpp"
#include "io/PcmFuncs.hpp"

namespace signal_estimator {

size_t PcmFormat::bits() const {
    return pcm_sample_bits(encoding);
}

size_t PcmFormat::bytes() const {
    return pcm_sample_bytes(encoding);
}

bool PcmFormat::is_int() const {
    return encoding != PcmEncoding::Float32 && encoding != PcmEncoding::Float64;
}

bool PcmFormat::operator==(const PcmFormat& other) const {
    return encoding == other.encoding && endian == other.endian;
}

bool PcmFormat::operator!=(const PcmFormat& other) const {
    return !(*this == other);
}

std::vector<PcmFormat> PcmFormat::supported_formats() {
    return pcm_supported_formats();
}

std::optional<PcmFormat> PcmFormat::from_string(const std::string& str) {
    return pcm_format_from_string(str);
}

std::string PcmFormat::to_string() const {
    return pcm_format_to_string(*this);
}

PcmFormat PcmFormat::from_alsa(int fmt) {
    return pcm_format_from_alsa(fmt);
}

int PcmFormat::to_alsa() const {
    return pcm_format_to_alsa(*this);
}

} // namespace signal_estimator
