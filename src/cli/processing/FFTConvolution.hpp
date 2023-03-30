// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "processing/FFTOps.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <complex>
#include <cstdint>

#include <kiss_fft.h>

namespace signal_estimator {

template <size_t frame_len, size_t taps_len, const std::array<float, taps_len>& taps_re>
class FFTConvolution {
public:
    FFTConvolution()
        : ksfft_state_fwd_(kiss_fft_alloc(fft_len, 0, 0, 0))
        , ksfft_state_inv_(kiss_fft_alloc(fft_len, 1, 0, 0)) {
        std::array<kiss_fft_cpx, fft_len> taps = {};
        std::transform(
            taps_re.rbegin(), taps_re.rend(), taps.begin(), [](float x) -> kiss_fft_cpx {
                return kiss_fft_cpx { x, 0.f };
            });

        std::fill(full_input_buf.begin(), full_input_buf.end(), kiss_fft_cpx { 0, 0 });
        std::fill(ftaps.begin(), ftaps.end(), kiss_fft_cpx { 0, 0 });
        std::fill(fft_out.begin(), fft_out.end(), kiss_fft_cpx { 0, 0 });
        std::fill(fir_tail.begin(), fir_tail.end(), kiss_fft_cpx { 0, 0 });

        kiss_fft(ksfft_state_fwd_, taps.data(), ftaps.data());
    }

    ~FFTConvolution() {
        free(ksfft_state_fwd_);
        free(ksfft_state_inv_);
    }

    FFTConvolution(const FFTConvolution&) = delete;
    FFTConvolution& operator=(const FFTConvolution&) = delete;

    void perform(const float* input_buf, float* out_buf, const size_t sz) {
        assert(sz <= frame_len);
        std::transform(input_buf, input_buf + sz, full_input_buf.begin(),
            [](float x) -> kiss_fft_cpx {
                return kiss_fft_cpx { x, 0 };
            });
        std::fill(
            full_input_buf.begin() + sz, full_input_buf.end(), kiss_fft_cpx { 0.f, 0.f });

        kiss_fft(ksfft_state_fwd_, full_input_buf.data(), fft_out.data());
        for (auto pout = fft_out.begin(), ptaps = ftaps.begin(); pout != fft_out.end();
             pout++, ptaps++) {
            *pout = *pout * *ptaps;
        }

        std::fill(
            full_input_buf.begin(), full_input_buf.end(), kiss_fft_cpx { 0.f, 0.f });
        kiss_fft(ksfft_state_inv_, fft_out.data(), full_input_buf.data());

        std::transform(full_input_buf.begin(), full_input_buf.begin() + sz, out_buf,
            [](kiss_fft_cpx& y) -> float { return y.r / float(fft_len); });

        const auto fir_tail_len_runtime_const = std::min(fir_tail_len, sz);
        for (size_t i = 0; i < fir_tail_len_runtime_const; i++)
            out_buf[i] += fir_tail[i].r / float(fft_len);
        if (sz < fir_tail_len) {
            std::copy(fir_tail.begin() + sz, fir_tail.end(), fir_tail.begin());
            std::fill(fir_tail.begin() + (fir_tail_len - sz), fir_tail.end(),
                kiss_fft_cpx { 0, 0 });
            for (size_t i = 0; i < fir_tail_len; ++i) {
                fir_tail[i].r += full_input_buf[sz + i].r;
                fir_tail[i].i += full_input_buf[sz + i].i;
            }
        } else {
            std::copy(full_input_buf.begin() + sz,
                full_input_buf.begin() + sz + fir_tail_len, fir_tail.begin());
        }
    }

private:
    kiss_fft_cfg ksfft_state_fwd_;
    kiss_fft_cfg ksfft_state_inv_;

    static constexpr size_t fir_tail_len = taps_len - 1;
    static constexpr size_t fft_len = frame_len + fir_tail_len + 1;

    std::array<kiss_fft_cpx, fft_len> full_input_buf = {};
    std::array<kiss_fft_cpx, fft_len> ftaps = {};
    std::array<kiss_fft_cpx, fft_len> fft_out = {};
    std::array<kiss_fft_cpx, fir_tail_len> fir_tail = {};
};

} // namespace signal_estimator
