// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "ConfigBuilder.hpp"
#include "DevInfoBuilder.hpp"
#include "StringSink.hpp"

#include "reports/TextReporter.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace se = signal_estimator;

using StringTextPrinter = se::BasicTextPrinter<StringSink>;
using StringTextReporter = se::BasicTextReporter<StringSink>;

struct TextReporterParams {
    se::Config config;
    se::DevInfo dev_info;
    std::string expected_result;
};

struct TextReporterTest : testing::TestWithParam<TextReporterParams> {
    StringSink string_sink;
};

TEST_P(TextReporterTest, LatencyReport) {
    const auto& [config, dev_info, expected_result] = GetParam();
    {
        StringTextPrinter text_printer(string_sink);
        StringTextReporter reporter(config, dev_info, text_printer);

        se::LatencyReport latency_report;
        latency_report.sw_hw = 1.2345;
        latency_report.hw = -2.3456;
        latency_report.hw_avg = 3.4567;
        reporter.report(latency_report);
    }

    EXPECT_THAT(string_sink.buffer, testing::StrEq(expected_result));
}

INSTANTIATE_TEST_SUITE_P(TextReporterSuite, TextReporterTest,
    testing::Values(
        TextReporterParams {
            ConfigBuilder {}.set_show_device_names(false).set_diff_inputs(true).build(),
            DevInfoBuilder {}.build(),
            std::string(
                "latency:  sw+hw   +1.23fms  hw   -2.35fms  hw_avg5   +3.46fms\n") },
        TextReporterParams {
            ConfigBuilder {}.set_show_device_names(true).set_diff_inputs(false).build(),
            DevInfoBuilder {}.build(),
            std::string(
                "latency:  sw+hw    1.23fms  hw   -2.35fms  hw_avg5    3.46fms\n") },
        TextReporterParams {
            ConfigBuilder {}.set_show_device_names(false).set_diff_inputs(false).build(),
            DevInfoBuilder {}.set_short_name("Test Device").build(),
            std::string(
                "latency:  sw+hw    1.23fms  hw   -2.35fms  hw_avg5    3.46fms\n") },
        TextReporterParams {
            ConfigBuilder {}.set_show_device_names(true).set_diff_inputs(false).build(),
            DevInfoBuilder {}.set_short_name("Test Device").build(),
            std::string("latency[Test Device]:  sw+hw    1.23fms  hw   -2.35fms  hw_avg5 "
                        "   3.46fms\n") }));
