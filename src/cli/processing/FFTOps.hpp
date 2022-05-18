// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <kiss_fft.h>

kiss_fft_cpx operator*(const kiss_fft_cpx& x, const kiss_fft_cpx& y);
