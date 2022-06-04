// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"
#include "SignalEstimator.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    if (SignalEstimator::find().isEmpty()) {
        NotFoundDialog w;
        w.exec();
        return 1;
    }

    MainWindow w;
    return a.exec();
}
