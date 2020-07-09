#include "JSONFormatter.hpp"
#include "Log.hpp"

namespace signal_estimator {

void JSONFormatter::report_latency(double sw_hw, double hw, int sma_window, double hw_avg) {
    if (first_output_) {
        printf("[\n");
        printf("  {\"sw+hw\": %7.3f, \"hw\": %7.3f, \"hw_avg%d\": %7.3f}", sw_hw,
        hw, sma_window, hw_avg);
        first_output_ = false;
    } else {
        printf(",\n");
        printf("  {\"sw+hw\": %7.3f, \"hw\": %7.3f, \"hw_avg%d\": %7.3f}", sw_hw,
        hw, sma_window, hw_avg);
    }
    fflush(stdout);
}

void JSONFormatter::report_losses(double loss_rate, int sma_window, double avg_loss_rate, double loss_ratio) {
    if (first_output_) {
        printf("[\n");
        printf("  {\"rate\": %5.1f, \"rate_avg%d\": %5.1f, \"ratio\": %6.2f%%}",
            loss_rate, sma_window, avg_loss_rate, loss_ratio);
        first_output_ = false;
    } else {
        printf(",\n");
        printf("  {\"rate\": %5.1f, \"rate_avg%d\": %5.1f, \"ratio\": %6.2f%%}",
            loss_rate, sma_window, avg_loss_rate, loss_ratio);
    }
    fflush(stdout);
}

JSONFormatter::JSONFormatter(){
    first_output_ = true;
}

JSONFormatter::~JSONFormatter(){
    if(!first_output_)
        printf("\n]\n");
}

} // namespace signal_estimator