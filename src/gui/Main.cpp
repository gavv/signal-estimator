// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Log.hpp"
#include "io/AlsaDeviceManager.hpp"

#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"
#include "SignalEstimator.hpp"

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <QApplication>

using namespace signal_estimator;

int main(int argc, char* argv[]) {
    se_log_init();

    QApplication a(argc, argv);

    if (SignalEstimator::find().isEmpty()) {
        NotFoundDialog w;
        w.exec();
        return 1;
    }

    AlsaDeviceManager device_manager;
    MainWindow w { device_manager };

    return a.exec();
}
