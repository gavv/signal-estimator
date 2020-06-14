#include "TextFormatter.hpp"
#include "Log.hpp"

namespace signal_estimator {

void TextFormatter::report_latency(double sw_hw, double hw, int sma_window, double hw_avg) {
    se_log_info("latency:  sw+hw %7.3fms  hw %7.3fms  hw_avg%d %7.3fms", sw_hw,
        hw, sma_window, hw_avg);
}

void TextFormatter::report_losses(double loss_rate, int sma_window, double avg_loss_rate, double loss_ratio) {
    se_log_info("losses:  rate %5.1f/sec  rate_avg%d %5.1f/sec  ratio %6.2f%%",
            loss_rate, sma_window, avg_loss_rate, loss_ratio);
}

}