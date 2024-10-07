// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/TextReporter.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace signal_estimator;

namespace {

struct MockConsole : Console {
    void write(const char* str) override {
        buffer += str;
    }

    void flush() override {
    }

    std::string buffer;
};

struct TextReporterParam {
    Config config;
    DevInfo dev_info;
    std::string expected_result;
};

struct TextReporterSuite : testing::TestWithParam<TextReporterParam> { };

TEST_P(TextReporterSuite, LatencyReport) {
    const auto& [config, dev_info, expected_result] = GetParam();
    MockConsole console;

    {
        TextPrinter printer(console);
        TextReporter reporter(config, dev_info, printer);

        LatencyReport latency_report;
        latency_report.sw_hw = 1.2345;
        latency_report.hw = -2.3456;
        latency_report.hw_avg = 3.4567;
        reporter.report(latency_report);
    }

    EXPECT_THAT(console.buffer, testing::StrEq(expected_result));
}

INSTANTIATE_TEST_SUITE_P(TextReporter, TextReporterSuite,
    testing::Values(
        TextReporterParam {
            Config {
                .diff_inputs = true,
            },
            DevInfo {},
            "latency:  sw+hw   +1.23ms  hw   -2.35ms  hw_avg5   +3.46ms\n",
        },
        TextReporterParam {
            Config {
                .show_device_names = true,
                .diff_inputs = false,
            },
            DevInfo {},
            "latency:  sw+hw    1.23ms  hw   -2.35ms  hw_avg5    3.46ms\n",
        },
        TextReporterParam {
            Config {
                .show_device_names = false,
                .diff_inputs = false,
            },
            DevInfo {
                .short_name = "Test Device",
            },
            "latency:  sw+hw    1.23ms  hw   -2.35ms  hw_avg5    3.46ms\n",
        },
        TextReporterParam {
            Config {
                .show_device_names = true,
                .diff_inputs = false,
            },
            DevInfo {
                .short_name = "Test Device",
            },
            "latency[Test Device]:  sw+hw    1.23ms  hw   -2.35ms  hw_avg5    3.46ms\n",
        }));

} // namespace
