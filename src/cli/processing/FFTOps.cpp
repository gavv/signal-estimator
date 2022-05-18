// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/FFTOps.hpp"

kiss_fft_cpx operator*(const kiss_fft_cpx& x, const kiss_fft_cpx& y) {
    kiss_fft_cpx res { x.r * y.r - x.i * y.i, x.r * y.i + x.i * y.r };
    return res;
}
