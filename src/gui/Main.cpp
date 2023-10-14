// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "sndio/AlsaDeviceManager.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"
#include "SignalEstimator.hpp"

#include <QApplication>

using namespace signal_estimator;

int main(int argc, char* argv[]) {

	auto err_logger = spdlog::stderr_color_mt<spdlog::async_factory>("stderr");
	spdlog::set_default_logger(err_logger);

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
