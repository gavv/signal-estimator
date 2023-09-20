// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "sndio/AlsaDeviceManager.hpp"

#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"
#include "SignalEstimator.hpp"

#include <QApplication>

using namespace signal_estimator;

int main(int argc, char* argv[]) {
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
