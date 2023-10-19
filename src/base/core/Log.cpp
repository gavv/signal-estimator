// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

void se_log_init() {
    auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

    sink->set_color(spdlog::level::critical, sink->red);
    sink->set_color(spdlog::level::err, sink->red);
    sink->set_color(spdlog::level::warn, sink->yellow);
    sink->set_color(spdlog::level::info, sink->green);
    sink->set_color(spdlog::level::debug, sink->reset);
    sink->set_color(spdlog::level::trace, sink->white);

    auto logger = std::make_shared<spdlog::logger>("default", sink);

    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%L%L] %^%v%$");
}
