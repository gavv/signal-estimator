// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

void init_log(int verbosity) {
    auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

    sink->set_color(spdlog::level::critical, sink->red);
    sink->set_color(spdlog::level::err, sink->red);
    sink->set_color(spdlog::level::warn, sink->yellow);
    sink->set_color(spdlog::level::info, sink->green);
    sink->set_color(spdlog::level::debug, sink->reset);
    sink->set_color(spdlog::level::trace, sink->white);

    auto logger = std::make_shared<spdlog::logger>("default", sink);

    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%L%L] %^%v%$");

    spdlog::level::level_enum log_level;
    switch (verbosity) {
    case 0:
        log_level = spdlog::level::warn;
        break;
    case 1:
        log_level = spdlog::level::info;
        break;
    case 2:
        log_level = spdlog::level::debug;
        break;
    default:
        log_level = spdlog::level::trace;
        break;
    }

    spdlog::set_level(log_level);
}
