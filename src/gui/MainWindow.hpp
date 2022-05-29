// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "Devices.hpp"
#include "NotFoundDialog.hpp"
#include "PointsBuffer.hpp"
#include "SignalEstimator.hpp"

#include <QMainWindow>
#include <QScopedPointer>
#include <QThread>
#include <QTimer>

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <qwt_text.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <thread>
#include <tuple>
#include <unistd.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_StartButton_released();
    void on_StopButton_clicked();

    void update_graphs();
    void read_graph_data();
    void check_proc();

private:
    QStringList set_up_program_();

    void set_update_plots_(bool f) {
        update_plots_ = f;
    }

    void run_estimator_();

    Ui::MainWindow* ui;
    bool update_plots_ = false;

    PointsBuffer out_data_;
    PointsBuffer in_data_;

    QScopedPointer<QwtPlotGrid> grid_ { new QwtPlotGrid() };

    QScopedPointer<QwtPlotCurve> inputCurve_ { new QwtPlotCurve(
        "Input signal (captured)") };
    QScopedPointer<QwtPlotCurve> outputCurve_ { new QwtPlotCurve(
        "Output signal (emitted)") };

    QTimer* timer_ = nullptr;
    QSharedPointer<QProcess> proc_;
};
