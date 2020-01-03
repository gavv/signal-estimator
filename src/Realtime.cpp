/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "Realtime.hpp"
#include "Log.hpp"

#include <sched.h>

namespace signal_estimator {

void set_realtime() {
    sched_param param {};
    param.sched_priority = sched_get_priority_max(SCHED_RR);

    if (sched_setscheduler(0, SCHED_RR, &param) == 0) {
        log_info("successfully enabled real-time scheduling policy");
    } else {
        log_info("can't enable real-time scheduling policy");
    }
}

} // namespace signal_estimator
