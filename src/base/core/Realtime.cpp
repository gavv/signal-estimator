// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Realtime.hpp"
#include "core/Log.hpp"

#include <sched.h>

namespace signal_estimator {

void make_realtime() {
    sched_param param {};
    param.sched_priority = sched_get_priority_max(SCHED_RR);

    if (sched_setscheduler(0, SCHED_RR, &param) == 0) {
        se_log_debug("successfully enabled real-time scheduling policy");
    } else {
        se_log_warn("can't enable real-time scheduling policy (try running with sudo)");
    }
}

} // namespace signal_estimator
