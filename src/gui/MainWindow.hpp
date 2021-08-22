#pragma once

#include "Devices.hpp"
#include "NotFoundDialog.hpp"
#include "PointsBuffer.hpp"
#include "SignalEstimator.hpp"

#include <QMainWindow>
#include <QThread>
#include <QTimer>

#include <qwt_plot_curve.h>
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
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_StartButton_released();
    void on_StopButton_clicked();

    void update_graphs();
    void read_graph_data();
    void check_proc();

private:
    QStringList set_up_program_();

    void set_update_plots_(bool f) {
        this->update_plots_ = f;
    }

    void run_estimator_();

    Ui::MainWindow* ui;
    bool update_plots_ = false;

    PointsBuffer out_data_;
    PointsBuffer in_data_;

    QwtPlotCurve* inputCurve_ = new QwtPlotCurve("Input Curve");
    QwtPlotCurve* outputCurve_ = new QwtPlotCurve("Output Curve");
    QTimer* timer_ = nullptr;
    QSharedPointer<QProcess> proc_;
};
