/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <cstdio>

#define log_error(fmt, ...)                                                              \
    fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__)

#define log_info(fmt, ...)                                                               \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__)
