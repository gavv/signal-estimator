// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"
#include "SignalEstimator.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // open error window if signal-estimator is not found
    if (find_signal_estimator().isEmpty()) {
        NotFoundDialog w;
        w.exec();
        return 1;
    }

    MainWindow w;
    return a.exec();
}
