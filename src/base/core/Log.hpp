// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <spdlog/spdlog.h>

#define se_log_error(fmt, ...) spdlog::error(fmt, ##__VA_ARGS__)
#define se_log_warn(fmt, ...) spdlog::warn(fmt, ##__VA_ARGS__)
#define se_log_info(fmt, ...) spdlog::info(fmt, ##__VA_ARGS__)
#define se_log_debug(fmt, ...) spdlog::debug(fmt, ##__VA_ARGS__)

void se_log_init();
