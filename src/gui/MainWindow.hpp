// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "io/IDeviceManager.hpp"

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

namespace signal_estimator {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(IDeviceManager& device_manager, QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_StartButton_released();
    void on_StopButton_clicked();

    void update_graphs();
    void read_graph_data();
    void show_error(QString error);

private:
    QStringList set_up_program_();

    void set_update_plots_(bool f);
    void update_latency_(LatencyResult latency);
    void update_losses_(LossesResult losses);
    void display_latency_text_();
    void display_losses_text_();
    void clear_results_text_();

    Ui::MainWindow* ui;
    bool update_plots_ = false;

    PointsBuffer out_data_;
    PointsBuffer in_data_;
    PointsBuffer data1_;
    PointsBuffer data2_;
    PointsBuffer data3_;

    QScopedPointer<QwtPlotGrid> grid1_ { new QwtPlotGrid() };
    QScopedPointer<QwtPlotGrid> grid2_ { new QwtPlotGrid() };
    QScopedPointer<QwtPlotGrid> grid3_ { new QwtPlotGrid() };

    QScopedPointer<QwtPlotCurve> inputCurve_ { new QwtPlotCurve(
        "Input signal (captured)") };
    QScopedPointer<QwtPlotCurve> outputCurve_ { new QwtPlotCurve(
        "Output signal (emitted)") };
    QScopedPointer<QwtPlotCurve> data1Curve_ { new QwtPlotCurve("") };
    QScopedPointer<QwtPlotCurve> data2Curve_ { new QwtPlotCurve("") };
    QScopedPointer<QwtPlotCurve> data3Curve_ { new QwtPlotCurve("") };

    QTimer* timer_ = nullptr;

    IDeviceManager& device_manager_;
    SignalEstimator* signal_estimator_ = nullptr;
};

} // namespace signal_estimator
