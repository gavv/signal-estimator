// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace signal_estimator {

// PCM sample encoding
enum class PcmEncoding {
    SInt8, //!< 8-bit signed integer.
    UInt8, //!< 8-bit unsigned integer.
    SInt16, //!< 16-bit signed integer.
    UInt16, //!< 16-bit unsigned integer.
    SInt18_3B, //!< 18-bit signed integer, in low bits of 3-byte container.
    UInt18_3B, //!< 18-bit unsigned integer, in low bits of 3-byte container.
    SInt20_3B, //!< 20-bit signed integer, in low bits of 3-byte container.
    UInt20_3B, //!< 20-bit unsigned integer, in low bits of 3-byte container.
    SInt20_4B, //!< 20-bit signed integer, in low bits of 4-byte container.
    UInt20_4B, //!< 20-bit unsigned integer, in low bits of 4-byte container.
    SInt24_3B, //!< 24-bit signed integer (3 bytes).
    UInt24_3B, //!< 24-bit unsigned integer (3 bytes).
    SInt24_4B, //!< 24-bit signed integer, in low bits of 4-byte container.
    UInt24_4B, //!< 24-bit unsigned integer, in low bits of 4-byte container.
    SInt32, //!< 32-bit signed integer.
    UInt32, //!< 32-bit unsigned integer.
    Float32, //!< 32-bit IEEE-754 float in range [-1.0; +1.0].
    Float64 //!< 64-bit IEEE-754 float in range [-1.0; +1.0].
};

// PCM sample endianess
enum class PcmEndian {
    Native, //!< Endian native to current CPU.
    Big, //!< Big endian.
    Little //!< Little endian.
};

// PCM format description
struct PcmFormat {
    PcmEncoding encoding { PcmEncoding::SInt16 };
    PcmEndian endian { PcmEndian::Native };

    // info
    size_t bits() const;
    size_t bytes() const;
    bool is_int() const;

    // equality
    bool operator==(const PcmFormat& other) const;
    bool operator!=(const PcmFormat& other) const;

    // format list
    static std::vector<PcmFormat> supported_formats();

    // strings
    static std::optional<PcmFormat> from_string(const std::string& str);
    std::string to_string() const;

    // alsa
    static PcmFormat from_alsa(int fmt);
    int to_alsa() const;
};

} // namespace signal_estimator
