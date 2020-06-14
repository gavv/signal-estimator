# pragma once

#include "IFormatter.hpp"

namespace signal_estimator {

class TextFormatter : public IFormatter {
    public: 
        void report_latency(double sw_hw, double hw, int sma_window, double hw_avg) override;
        void report_losses(double loss_rate, int sma_window, double avg_loss_rate, double loss_ratio) override;
};

} // namespace signal_estimator