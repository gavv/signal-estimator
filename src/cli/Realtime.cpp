// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Realtime.hpp"
#include "Log.hpp"

#include <sched.h>

namespace signal_estimator {

void set_realtime() {
    sched_param param {};
    param.sched_priority = sched_get_priority_max(SCHED_RR);

    if (sched_setscheduler(0, SCHED_RR, &param) == 0) {
        se_log_info("successfully enabled real-time scheduling policy");
    } else {
        se_log_info("can't enable real-time scheduling policy");
    }
}

} // namespace signal_estimator
