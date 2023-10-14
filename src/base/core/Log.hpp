// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "spdlog/spdlog.h"

#define se_log_error(fmt, ...) spdlog::error(fmt "\n", ##__VA_ARGS__)

#define se_log_info(fmt, ...) spdlog::info(fmt "\n", ##__VA_ARGS__)
