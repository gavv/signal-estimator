// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <spdlog/spdlog.h>

#define se_log_error(fmt, ...) spdlog::error(fmt, ##__VA_ARGS__)

#define se_log_info(fmt, ...) spdlog::info(fmt, ##__VA_ARGS__)
