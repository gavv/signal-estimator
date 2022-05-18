// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdio>

#define se_log_error(fmt, ...) fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__)

#define se_log_info(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
