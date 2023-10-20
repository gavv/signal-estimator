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

protected slots:
    void closeEvent(QCloseEvent*) override;

private slots:
    void on_start();
    void on_stop();
    void on_DurationCheckBox_stateChanged(int state);

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

    QScopedPointer<QwtPlotGrid> grid_ { new QwtPlotGrid() };

    QScopedPointer<QwtPlotCurve> inputCurve_ { new QwtPlotCurve(
        "Input signal (captured)") };
    QScopedPointer<QwtPlotCurve> outputCurve_ { new QwtPlotCurve(
        "Output signal (emitted)") };

    QTimer* timer_ = nullptr;

    IDeviceManager& device_manager_;
    SignalEstimator* signal_estimator_ = nullptr;
};

} // namespace signal_estimator
