#!env python3
import builtins
import math
import os
import sys

try:
    import jinja2
except ImportError:
    print('''
error: can't import python module "jinja2", install it using "pip3 install Jinja2"
'''.strip(), file=sys.stderr)
    exit(1)

def compute_minmax(enc):
    suffix = ''
    if not enc['is_signed']:
        suffix += 'u'
    if enc['width'] >= 32:
        suffix += 'l'
    if enc['width'] >= 64:
        suffix += 'l'

    if enc['is_signed']:
        power = pow(2, enc['width']-1) - 1

        min_val = f'-{power}{suffix} - 1'
        max_val = f'{power}{suffix}'
    else:
        power = pow(2, enc['width']) - 1

        min_val = f'0{suffix}'
        max_val = f'{power}{suffix}'

    return min_val, max_val

def compute_masks(enc):
    value_mask = hex(int('1' * enc['width'], 2))
    if not enc['is_signed']:
        value_mask += 'u'

    if enc['is_signed']:
        sign_mask = hex(int('1' + '0' * (enc['width']-1), 2))
    else:
        sign_mask = None

    if enc['is_signed'] and enc['unpacked_width'] > enc['width']:
        lsb_mask = hex(
            int(
                ('1' * (enc['unpacked_width'] - enc['width']) +
                '0' * enc['width']),
                2))
    else:
        lsb_mask = None

    return value_mask, sign_mask, lsb_mask

def compute_octets(enc):
    return (math.ceil(enc['width'] / 8),
            math.ceil(enc['packed_width'] / 8),
            math.ceil(enc['unpacked_width'] / 8))

def alsa_name(enc, end):
    name = enc['alsa_format']
    if end != 'Native':
        if '_' not in name:
            name += '_'
        if end == 'Big':
            name += 'BE'
        if end == 'Little':
            name += 'LE'
    return name

main_encoding = 'SInt16'

encodings = [
    {
        'encoding': 'SInt8',
        'signed_encoding': 'SInt8',
        'type': 'int8_t',
        'signed_type': 'int8_t',
        'unsigned_type': 'uint8_t',
        'alsa_format': 'S8',
        'is_integer': True,
        'is_signed': True,
        'width': 8,
        'packed_width': 8,
        'unpacked_width': 8,
    },
    {
        'encoding': 'UInt8',
        'signed_encoding': 'SInt8',
        'type': 'uint8_t',
        'signed_type': 'int8_t',
        'unsigned_type': 'uint8_t',
        'alsa_format': 'U8',
        'is_integer': True,
        'is_signed': False,
        'width': 8,
        'packed_width': 8,
        'unpacked_width': 8,
    },
    {
        'encoding': 'SInt16',
        'signed_encoding': 'SInt16',
        'type': 'int16_t',
        'signed_type': 'int16_t',
        'unsigned_type': 'uint16_t',
        'alsa_format': 'S16',
        'is_integer': True,
        'is_signed': True,
        'width': 16,
        'packed_width': 16,
        'unpacked_width': 16,
    },
    {
        'encoding': 'UInt16',
        'signed_encoding': 'SInt16',
        'type': 'uint16_t',
        'signed_type': 'int16_t',
        'unsigned_type': 'uint16_t',
        'alsa_format': 'U16',
        'is_integer': True,
        'is_signed': False,
        'width': 16,
        'packed_width': 16,
        'unpacked_width': 16,
    },
    {
        'encoding': 'SInt18_3B',
        'signed_encoding': 'SInt18_3B',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S18_3',
        'is_integer': True,
        'is_signed': True,
        'width': 18,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt18_3B',
        'signed_encoding': 'SInt18_3B',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U18_3',
        'is_integer': True,
        'is_signed': False,
        'width': 18,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'SInt20_3B',
        'signed_encoding': 'SInt20_3B',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S20_3',
        'is_integer': True,
        'is_signed': True,
        'width': 20,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt20_3B',
        'signed_encoding': 'SInt20_3B',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U20_3',
        'is_integer': True,
        'is_signed': False,
        'width': 20,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'SInt20_4B',
        'signed_encoding': 'SInt20_4B',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S20',
        'is_integer': True,
        'is_signed': True,
        'width': 20,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt20_4B',
        'signed_encoding': 'SInt20_4B',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U20',
        'is_integer': True,
        'is_signed': False,
        'width': 20,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'SInt24_3B',
        'signed_encoding': 'SInt24_3B',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S24_3',
        'is_integer': True,
        'is_signed': True,
        'width': 24,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt24_3B',
        'signed_encoding': 'SInt24_3B',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U24_3',
        'is_integer': True,
        'is_signed': False,
        'width': 24,
        'packed_width': 24,
        'unpacked_width': 32,
    },
    {
        'encoding': 'SInt24_4B',
        'signed_encoding': 'SInt24_4B',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S24',
        'is_integer': True,
        'is_signed': True,
        'width': 24,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt24_4B',
        'signed_encoding': 'SInt24_4B',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U24',
        'is_integer': True,
        'is_signed': False,
        'width': 24,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'SInt32',
        'signed_encoding': 'SInt32',
        'type': 'int32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'S32',
        'is_integer': True,
        'is_signed': True,
        'width': 32,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'UInt32',
        'signed_encoding': 'SInt32',
        'type': 'uint32_t',
        'signed_type': 'int32_t',
        'unsigned_type': 'uint32_t',
        'alsa_format': 'U32',
        'is_integer': True,
        'is_signed': False,
        'width': 32,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'Float32',
        'signed_encoding': 'Float32',
        'type': 'float',
        'signed_type': 'float',
        'alsa_format': 'FLOAT',
        'is_integer': False,
        'is_signed': True,
        'width': 32,
        'packed_width': 32,
        'unpacked_width': 32,
    },
    {
        'encoding': 'Float64',
        'signed_encoding': 'Float64',
        'type': 'double',
        'signed_type': 'double',
        'alsa_format': 'FLOAT64',
        'is_integer': False,
        'is_signed': True,
        'width': 64,
        'packed_width': 64,
        'unpacked_width': 64,
    },
]

endians = [
    'Native',
    'Big',
    'Little',
]

types = [
    ('int8_t', 1),
    ('uint8_t', 1),
    ('int16_t', 2),
    ('uint16_t', 2),
    ('int32_t', 4),
    ('uint32_t', 4),
    ('int64_t', 8),
    ('uint64_t', 8),
    ('float', 4),
    ('double', 8),
]

for enc in encodings:
    enc['min'] = f"pcm_{enc['encoding'].lower()}_min"
    enc['max'] = f"pcm_{enc['encoding'].lower()}_max"

    enc['signed_min'] = f"pcm_{enc['signed_encoding'].lower()}_min"
    enc['signed_max'] = f"pcm_{enc['signed_encoding'].lower()}_max"

    enc['min_value'], enc['max_value'] = compute_minmax(enc)

    enc['value_mask'], enc['sign_mask'], enc['lsb_mask'] = \
      compute_masks(enc)

    enc['significant_octets'], enc['packed_octets'], enc['unpacked_octets'] = \
      compute_octets(enc)

env = jinja2.Environment(
    trim_blocks=True,
    lstrip_blocks=True,
    undefined = jinja2.StrictUndefined)

template = env.from_string('''
// Copyright (c) Signal Estimator authors
// Licensed under MIT

// Generated by generate_pcm.py. DO NOT EDIT!

#pragma once

#include "io/AlsaConstants.hpp"
#include "io/PcmFormat.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
// Big-endian CPU
#define PCM_BIG_ENDIAN
#else
// Little-endian CPU
#define PCM_LITTLE_ENDIAN
#endif

namespace signal_estimator {

{% for enc in encodings %}
{% if enc.is_integer %}
// {{ enc.encoding }} value range
const {{ enc.type }} {{ enc.min }} = {{ enc.min_value }};
const {{ enc.type }} {{ enc.max }} = {{ enc.max_value }};

{% endif %}
{% endfor %}
// Convert between signed and unsigned samples
template <PcmEncoding>
struct pcm_sign_converter;

{% for enc in encodings %}
{% if enc.is_integer %}
// Convert {{ enc.encoding }} from/to signed/unsigned
template <>
struct pcm_sign_converter<PcmEncoding::{{ enc.encoding }}> {
{% if enc.is_signed %}
    // {{ enc.encoding }} from unsigned value
    static inline {{ enc.signed_type }} from_unsigned({{ enc.unsigned_type }} arg) {
        if (arg < {{ enc.unsigned_type }}({{ enc.signed_max }}) + 1) {
            return {{ enc.signed_type }}(arg) - {{ enc.signed_max }} - 1;
        }
        return {{ enc.signed_type }}(arg - {{ enc.unsigned_type }}({{ enc.signed_max }}) - 1);
    }

    // {{ enc.encoding }} to unsigned value
    static inline {{ enc.unsigned_type }} to_unsigned({{ enc.signed_type }} arg) {
        if (arg >= 0) {
            return {{ enc.unsigned_type }}(arg) + {{ enc.signed_max }} + 1;
        }
        return {{ enc.unsigned_type }}(arg + {{ enc.signed_max }} + 1);
    }
{% else %}
    // {{ enc.encoding }} from signed value
    static inline {{ enc.unsigned_type }} from_signed({{ enc.signed_type }} arg) {
        if (arg >= 0) {
            return {{ enc.unsigned_type }}(arg) + {{ enc.signed_max }} + 1;
        }
        return {{ enc.unsigned_type }}(arg + {{ enc.signed_max }} + 1);
    }

    // {{ enc.encoding }} to signed value
    static inline {{ enc.signed_type }} to_signed({{ enc.unsigned_type }} arg) {
        if (arg >= {{ enc.unsigned_type }}({{ enc.signed_max }}) + 1) {
            return {{ enc.signed_type }}(arg \
- {{ enc.unsigned_type }}({{ enc.signed_max }}) - 1);
        }
        return {{ enc.signed_type }}(arg - {{ enc.unsigned_type }}({{ enc.signed_max }}) - 1);
    }
{% endif %}
};

{% endif %}
{% endfor %}
// Convert between unpacked encodings
template <PcmEncoding InEnc, PcmEncoding OutEnc>
struct pcm_encoding_converter;

{% for out in encodings %}
{% for in in encodings %}
{% if out.encoding == main_encoding or in.encoding == main_encoding %}
{% set both_unsigned = not in.is_signed and not out.is_signed %}
// Convert {{ in.encoding }} to {{ out.encoding }}
template <>
struct pcm_encoding_converter<PcmEncoding::{{ in.encoding }}, \
PcmEncoding::{{ out.encoding }}> {
    static inline {{ out.type }} convert({{ in.type }} arg) {
{% if in.encoding == out.encoding %}
        return arg;
{% else %}
{% if not in.is_signed and not both_unsigned %}
        // convert to signed
        {{ in.signed_type }} in = \
pcm_sign_converter<PcmEncoding::{{ in.encoding }}>::to_signed(arg);
{% else %}
        {{ in.type }} in = arg;
{% endif %}

{% if both_unsigned %}
        {{ out.type }} out;
{% else %}
        {{ out.signed_type }} out;
{% endif %}
{% if not out.is_integer and not in.is_integer %}
        // float to float
        out = {{ out.type }}(in);
{% elif not out.is_integer and in.is_integer %}
        // integer to float
        out = {{ out.type }}(in * (1.0 / ((double){{ in.signed_max }} + 1.0)));
{% elif out.is_integer and not in.is_integer %}
        // float to integer
        const double d = double(in) * ((double){{ out.signed_max }} + 1.0);
        if (d < {{ out.signed_min }}) {
            // clip
            out = {{ out.signed_min }};
        } else if (d >= (double){{ out.signed_max }} + 1.0) {
            // clip
            out = {{ out.signed_max }};
        } else {
            out = {{ out.signed_type }}(d);
        }
{% elif out.width == in.width %}
        out = in;
{% elif out.width < in.width and both_unsigned %}
        // downscale unsigned integer
        out = {{ out.type }}(in >> {{ in.width - out.width }});
{% elif out.width < in.width and not both_unsigned %}
        // downscale signed integer
        if (in > {{ in.signed_type }}({{ in.signed_max }} - \
({{ in.signed_type }}(1) << {{ in.width - out.width - 1 }}))) {
            // clip
            out = {{ out.signed_max }};
        } else {
            out = {{ out.signed_type }}({{ in.unsigned_type }}(in + \
({{ in.signed_type }}(1) << {{ in.width - out.width - 1 }})) >> {{ in.width - out.width }});
        }
{% elif out.width > in.width and both_unsigned %}
        // upscale unsigned integer
        out = {{ out.type }}({{ out.type }}(in) << {{ out.width - in.width }});
{% elif out.width > in.width and not both_unsigned %}
        // upscale signed integer
        out = {{ out.signed_type }}({{ out.unsigned_type }}(in) << {{ out.width - in.width }});
{% endif %}

{% if not out.is_signed and not both_unsigned %}
        // convert to unsigned
        return pcm_sign_converter<PcmEncoding::{{ out.encoding }}>::from_signed(out);
{% else %}
        return out;
{% endif %}
{% endif %}
    }
};

{% endif %}
{% endfor %}
{% endfor %}
// N-byte native-endian packed octet array
template <size_t N>
struct pcm_octets;

{% for size in [1, 2, 4, 8] %}
// {{ size }}-byte native-endian packed octet array
template <>
struct pcm_octets<{{ size }}> {
#ifdef PCM_BIG_ENDIAN
{% for n in reversed(range(size)) %}
    uint8_t octet{{ n }};
{% endfor %}
#else
{% for n in range(size) %}
    uint8_t octet{{ n }};
{% endfor %}
#endif
};

{% endfor %}
// N-byte native-endian sample
template <class T>
struct pcm_sample;

{% for type, size in types %}
// {{ type }} native-endian sample
template <>
struct pcm_sample<{{ type }}> {
    union {
        {{ type }} value;
        pcm_octets<{{ size }}> octets;
    };
};

{% endfor %}

// Sample packer / unpacker
template <PcmEncoding, PcmEndian>
struct pcm_packer;

{% for enc in encodings %}
{% for endian in ['Big', 'Little'] %}
// {{ enc.encoding }} {{ endian }}-Endian packer / unpacker
template <>
struct pcm_packer<PcmEncoding::{{ enc.encoding }}, PcmEndian::{{ endian }}> {
    // Pack next sample to buffer
    static inline void pack(uint8_t* buffer, {{ enc.type }} arg) {
        // native-endian view of octets
        pcm_sample<{{ enc.type }}> p;
        p.value = arg;

{% if enc.width < enc.packed_width %}
        // zeroise padding bits
        p.value &= {{ enc.value_mask }};

{% endif %}
        // write in {{ endian.lower() }}-endian order
{% for n in range(enc.packed_octets) %}
{% if endian == 'Big' %}
{% set n = enc.packed_octets - n - 1 %}
{% endif %}
        *(buffer++) = p.octets.octet{{ n }};
{% endfor %}
    }

    // Unpack next sample from buffer
    static inline {{ enc.type }} unpack(const uint8_t* buffer) {
        // native-endian view of octets
        pcm_sample<{{ enc.type }}> p;

        // read in {{ endian.lower() }}-endian order
{% for n in range(enc.unpacked_octets) %}
{% if endian == 'Big' %}
{% set n = enc.unpacked_octets - n - 1 %}
{% endif %}
{% if n >= enc.packed_octets %}
        p.octets.octet{{ n }} = 0;
{% elif enc.packed_width % 8 == 0 %}
        p.octets.octet{{ n }} = *(buffer++);
{% endif %}
{% endfor %}

{% if enc.width < enc.packed_width %}
        // zeroise padding bits
        p.value &= {{ enc.value_mask }};

{% endif %}
{% if enc.is_signed and enc.width < enc.unpacked_width %}
        if (p.value & {{ enc.sign_mask }}) {
            // sign extension
            p.value |= ({{ enc.type }}){{ enc.lsb_mask }};
        }

{% endif %}
        return p.value;
    }
};

{% endfor %}
{% endfor %}
// Map encoding and endian of samples
template <PcmEncoding InEnc, PcmEncoding OutEnc, PcmEndian InEnd, PcmEndian OutEnd>
struct pcm_mapper {
    static inline void map(const uint8_t* in_sample, uint8_t* out_sample) {
        pcm_packer<OutEnc, OutEnd>::pack(out_sample,
            pcm_encoding_converter<InEnc, OutEnc>::convert(
                pcm_packer<InEnc, InEnd>::unpack(in_sample)));
    }
};

// Sample mapping function
typedef void (*pcm_mapper_func_t)(const uint8_t* in_data, uint8_t* out_data);

// Select mapper function
template <PcmEncoding InEnc, PcmEncoding OutEnc, PcmEndian InEnd, PcmEndian OutEnd>
pcm_mapper_func_t pcm_mapper_func() {
    return &pcm_mapper<InEnc, OutEnc, InEnd, OutEnd>::map;
}

// Select mapper function
template <PcmEncoding InEnc, PcmEncoding OutEnc>
pcm_mapper_func_t pcm_mapper_func(PcmEndian in_endian, PcmEndian out_endian) {
    switch (in_endian) {
{% for in_end in endians %}
    case PcmEndian::{{ in_end }}:
{% if in_end == 'Native' %}
#ifdef PCM_BIG_ENDIAN
        return pcm_mapper_func<InEnc, OutEnc>(PcmEndian::Big, out_endian);
#else
        return pcm_mapper_func<InEnc, OutEnc>(PcmEndian::Little, out_endian);
#endif
{% else %}
        switch (out_endian) {
{% for out_end in endians %}
        case PcmEndian::{{ out_end }}:
{% if out_end == 'Native' %}
#ifdef PCM_BIG_ENDIAN
            return pcm_mapper_func<InEnc, OutEnc>(in_endian, PcmEndian::Big);
#else
            return pcm_mapper_func<InEnc, OutEnc>(in_endian, PcmEndian::Little);
#endif
{% else %}
            return pcm_mapper_func<InEnc, OutEnc, PcmEndian::{{ in_end }}, PcmEndian::{{ out_end }}>();
{% endif %}
{% endfor %}
        }
        break;
{% endif %}
{% endfor %}
    }

    return nullptr;
}

// Select mapper function
inline pcm_mapper_func_t pcm_mapper_func(PcmEncoding in_encoding,
    PcmEncoding out_encoding, PcmEndian in_endian, PcmEndian out_endian) {
    if (in_encoding == PcmEncoding::{{ main_encoding }}) {
        switch (out_encoding) {
{% for e in encodings %}
        case PcmEncoding::{{ e.encoding }}:
            return pcm_mapper_func<PcmEncoding::{{ main_encoding }}, PcmEncoding::{{ e.encoding }}>(
                in_endian, out_endian);
{% endfor %}
        }
    }

    if (out_encoding == PcmEncoding::{{ main_encoding }}) {
        switch (in_encoding) {
{% for e in encodings %}
        case PcmEncoding::{{ e.encoding }}:
            return pcm_mapper_func<PcmEncoding::{{ e.encoding }}, PcmEncoding::{{ main_encoding }}>(
                in_endian, out_endian);
{% endfor %}
        }
    }

    return nullptr;
}

// Get number of bits per sample in packed format
inline size_t pcm_sample_bits(PcmEncoding encoding) {
    switch (encoding) {
{% for e in encodings %}
    case PcmEncoding::{{ e.encoding }}:
        return {{ e.width }};
{% endfor %}
    }
    return 0;
}

// Get number of bits per sample in packed format
inline size_t pcm_sample_bytes(PcmEncoding encoding) {
    switch (encoding) {
{% for e in encodings %}
    case PcmEncoding::{{ e.encoding }}:
        return {{ e.packed_width // 8 }};
{% endfor %}
    }
    return 0;
}

// List of all formats
inline std::vector<PcmFormat> pcm_supported_formats() {
    return {
{% for enc in encodings %}
{% for end in endians %}
{% if enc.width != 8 or end == 'Native' %}
        PcmFormat{ PcmEncoding::{{ enc.encoding }}, PcmEndian::{{ end }} },
{% endif %}
{% endfor %}
{% endfor %}
    };
}

// Format from string
inline std::optional<PcmFormat> pcm_format_from_string(const std::string& str) {
{% for enc in encodings %}
{% for end in endians %}
    if (str == "{{ alsa_name(enc, end).lower() }}") {
        return PcmFormat{ PcmEncoding::{{ enc.encoding }}, PcmEndian::{{ end }} };
    }
{% endfor %}
{% endfor %}
    return {};
}

// Format to string
inline std::string pcm_format_to_string(const PcmFormat& format) {
    switch (format.encoding) {
{% for enc in encodings %}
    case PcmEncoding::{{ enc.encoding }}:
        switch (format.endian) {
{% for end in endians %}
        case PcmEndian::{{ end }}:
            return "{{ alsa_name(enc, end).lower() }}";
{% endfor %}
        }
        break;
{% endfor %}
    }
    return "";
}

// Format from ALSA
inline PcmFormat pcm_format_from_alsa(int format) {
    switch (format) {
{% for enc in encodings %}
{% for end in endians %}
{% if enc.width == 8 and end == 'Native' %}
    case SND_PCM_FORMAT_{{ alsa_name(enc, 'Native') }}:
#ifdef PCM_BIG_ENDIAN
        return PcmFormat{ PcmEncoding::{{ enc.encoding }}, PcmEndian::Big };
#else
        return PcmFormat{ PcmEncoding::{{ enc.encoding }}, PcmEndian::Little };
#endif
{% elif enc.width != 8 and end != 'Native' %}
    case SND_PCM_FORMAT_{{ alsa_name(enc, end) }}:
        return PcmFormat{ PcmEncoding::{{ enc.encoding }}, PcmEndian::{{ end }} };
{% endif %}
{% endfor %}
{% endfor %}
    }
    return {};
}

// Format to ALSA
inline int pcm_format_to_alsa(const PcmFormat& format) {
    switch (format.encoding) {
{% for enc in encodings %}
    case PcmEncoding::{{ enc.encoding }}:
        switch (format.endian) {
{% for end in endians %}
        case PcmEndian::{{ end }}:
{% if enc.width == 8 %}
            return SND_PCM_FORMAT_{{ alsa_name(enc, 'Native') }};
{% elif end == 'Native' %}
#ifdef PCM_BIG_ENDIAN
            return SND_PCM_FORMAT_{{ alsa_name(enc, 'Big') }};
#else
            return SND_PCM_FORMAT_{{ alsa_name(enc, 'Little') }};
#endif
{% else %}
            return SND_PCM_FORMAT_{{ alsa_name(enc, end) }};
{% endif %}
{% endfor %}
        }
        break;
{% endfor %}
    }
    return SND_PCM_FORMAT_UNKNOWN;
}

} // namespace signal_estimator
'''.strip())

text = template.render(
    **dict(list(globals().items()) + list(builtins.__dict__.items())),
    )

os.chdir(os.path.join(
    os.path.dirname(os.path.abspath(__file__)), '..'))

cpp_path = 'src/base/io/PcmFuncs.hpp'
with open(cpp_path, 'w', encoding = 'utf-8') as fp:
    fp.write(text)
    fp.write('\n')
