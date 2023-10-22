// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/JsonReporter.hpp"

#include <cstdio>

namespace signal_estimator {

JsonReporter::JsonReporter() {
    first_output_ = true;
}

JsonReporter::~JsonReporter() {
    if (!first_output_) {
        printf("\n]\n");
    }
}

void JsonReporter::report_latency(double sw_hw, double hw, double hw_avg, int) {
    if (first_output_) {
        first_output_ = false;
        printf("[\n");
    } else {
        printf(",\n");
    }
    printf("  {\"sw_hw\": %f, \"hw\": %f, \"hw_avg\": %f}", sw_hw, hw, hw_avg);
    fflush(stdout);
}

void JsonReporter::report_losses(
    double loss_rate, double avg_loss_rate, double loss_ratio, int) {
    if (first_output_) {
        first_output_ = false;
        printf("[\n");
    } else {
        printf(",\n");
    }
    printf("  {\"rate\": %f, \"rate_avg\": %f, \"ratio\": %f}", loss_rate, avg_loss_rate,
        loss_ratio);
    fflush(stdout);
}

void JsonReporter::report_jitter(double swdev_avg, double swdev_per, double hwdev_avg,
    double hwdev_per, double hwbuf_avg, double hwbuf_per, int) {
    if (first_output_) {
        first_output_ = false;
        printf("[\n");
    } else {
        printf(",\n");
    }
    printf("  {\"swdev_avg\": %f, \"swdev_per\": %f,"
           " \"hwdev_avg\": %f, \"hwdev_per\": %f,"
           " \"hwbuf_avg\": %f, \"hwbuf_per\": %f}",
        swdev_avg, swdev_per, hwdev_avg, hwdev_per, hwbuf_avg, hwbuf_per);
    fflush(stdout);
}

} // namespace signal_estimator
