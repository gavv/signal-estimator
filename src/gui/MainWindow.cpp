// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "MainWindow.hpp"
#include "RightClickPickerMachine.hpp"
#include "ui_MainWindow.h"

#include <QCheckBox>
#include <QList>
#include <QPen>

#include <qwt_legend.h>
#include <qwt_picker_machine.h>

namespace signal_estimator {

MainWindow::MainWindow(IDeviceManager& device_manager, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , device_manager_(device_manager)
    , signal_estimator_(new SignalEstimator(this)) {
    ui->setupUi(this);

    connect(signal_estimator_, &SignalEstimator::error, this, &MainWindow::show_error);
    connect(signal_estimator_, &SignalEstimator::can_read, this,
        &MainWindow::read_graph_data);

    ui->OutputSig->setCanvasBackground(Qt::white);

    outputCurve_->setPen(QColor(0x1f77b4));
    outputCurve_->attach(ui->OutputSig);

    inputCurve_->setPen(QColor(0xff7f0e));
    inputCurve_->attach(ui->OutputSig);

    ui->OutputSig->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

    grid_->enableX(false);
    grid_->enableY(false);
    grid_->setMajorPen(Qt::black, 0.0, Qt::DotLine);
    grid_->attach(ui->OutputSig);

    connect(ui->GridCheckBox, &QCheckBox::stateChanged, [this](int state) {
        const bool enable = (state == Qt::Checked);
        grid_->enableX(enable);
        grid_->enableY(enable);
        ui->OutputSig->replot();
    });

    QwtPlotPicker* zoomInPicker = new QwtPlotPicker(ui->OutputSig->canvas());
    zoomInPicker->setStateMachine(new QwtPickerDragRectMachine());
    zoomInPicker->setRubberBand(QwtPicker::RectRubberBand);
    zoomInPicker->setRubberBandPen(QPen(QColor(Qt::red)));
    connect(zoomInPicker, qOverload<const QRectF&>(&QwtPlotPicker::selected),
        [this](const QRectF& rect) {
            ui->OutputSig->setAxisScale(
                QwtPlot::xBottom, rect.topLeft().x(), rect.bottomRight().x());
            ui->OutputSig->setAxisScale(
                QwtPlot::yLeft, rect.topLeft().y(), rect.bottomLeft().y());
            ui->OutputSig->replot();
        });

    QwtPlotPicker* zoomOutPicker = new QwtPlotPicker(ui->OutputSig->canvas());
    zoomOutPicker->setStateMachine(new RightClickPickerMachine());
    connect(zoomOutPicker, qOverload<const QPointF&>(&QwtPlotPicker::selected),
        [this](const QPointF&) {
            ui->OutputSig->setAxisAutoScale(QwtPlot::xBottom);
            ui->OutputSig->setAxisAutoScale(QwtPlot::yLeft);
            ui->OutputSig->replot();
        });

    QwtPlotPicker* trackPicker = new QwtPlotPicker(ui->OutputSig->canvas());
    trackPicker->setStateMachine(new QwtPickerTrackerMachine());
    connect(trackPicker, &QwtPlotPicker::moved, [this](const QPointF& pos) {
        ui->CursorPositionLabel->setText(QString("(%1, %2)")
                                             .arg(QString::number(pos.x()))
                                             .arg(QString::number(pos.y())));
    });

    const std::vector<std::string> in_devices = device_manager_.get_input_devices();
    const std::vector<std::string> out_devices = device_manager_.get_output_devices();

    auto to_list = [](const std::vector<std::string>& device_names) {
        QList<QString> result;
        result.reserve(static_cast<int>(device_names.size()));
        for (const std::string& device_name : device_names) {
            result.push_back(QString::fromStdString(device_name));
        }
        return result;
    };

    ui->InputDevices->addItems(to_list(in_devices));
    ui->OutputDevices->addItems(to_list(out_devices));

    show();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_StartButton_released() {
    QStringList args = set_up_program_();

    set_update_plots_(true); // must be true to update graphs

    // clear old data when the start button is pressed
    in_data_.clear_buf();
    out_data_.clear_buf();

    // reset graphs
    ui->OutputSig->updateAxes();
    ui->OutputSig->replot();

    if (!timer_) {
        timer_ = new QTimer(this);
        timer_->setInterval(20);
        timer_->connect(timer_, &QTimer::timeout, this, &MainWindow::update_graphs);
    }

    if (signal_estimator_->start(args)) {
        ui->ErrorLabel->setText("");
    } else {
        ui->ErrorLabel->setText(QString("Failed to open signal-estimator"));
    }

    set_update_plots_(true);
    timer_->start();
}

void MainWindow::on_StopButton_clicked() {
    signal_estimator_->stop();

    set_update_plots_(false);
}

void MainWindow::update_graphs() {
    if (auto latencyValues = signal_estimator_->latencyUpdate()) {
        update_latency(*latencyValues);
    }

    if (!update_plots_) {
        return;
    }
    // update both input and output signal graphs
    QVector<QPointF> in_current = in_data_.get_current_points();
    QVector<QPointF> out_current = out_data_.get_current_points();

    if (in_current.isEmpty() || out_current.isEmpty()) {
        return;
    }

    auto xMax = std::max(in_current.back().x(), out_current.back().x());
    auto xMin = xMax - out_data_.size_milliseconds();

    inputCurve_->setSamples(in_current);
    outputCurve_->setSamples(out_current);

    ui->OutputSig->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    ui->OutputSig->replot();
}

void MainWindow::read_graph_data() {
    while (auto entry = signal_estimator_->read()) {
        auto [pt, type] = *entry;

        if (type == PointType::Input) {
            in_data_.append_point(pt);
        } else if (type == PointType::Output) {
            out_data_.append_point(pt);
        }
    }
}

void MainWindow::show_error(QString error) {
    ui->ErrorLabel->setText(error);
}

QStringList MainWindow::set_up_program_() {
    QStringList list;
    QString t;

    list.append("-m");
    list.append(ui->Modes->currentText());

    t = ui->OutputDevices->currentText();
    list.append("-o");
    list.append(
        QString::fromStdString(device_manager_.format_device_name(t.toStdString())));

    t = ui->InputDevices->currentText();
    list.append("-i");
    list.append(
        QString::fromStdString(device_manager_.format_device_name(t.toStdString())));

    t = ui->SampleRate->cleanText();
    list.append("-r");
    list.append(t);

    t = ui->NumChannels->cleanText();
    list.append("-c");
    list.append(t);

    t = ui->SignalVolume->cleanText();
    list.append("-v");
    list.append(t);

    t = ui->PRB->cleanText();
    list.append("-p");
    list.append(t);

    t = ui->RBS->cleanText();
    list.append("-l");
    list.append(t);

    t = ui->Duration->cleanText();
    list.append("-d");
    list.append(t);

    // both of these options have to be stdout because we use a pipe
    // that reads from signal_estimator stdout
    list << "--dump-out"
         << "-";
    list << "--dump-in"
         << "-";

    t = ui->DC->cleanText();
    list.append("--dump-compression");
    list.append(t);

    t = ui->SMA->cleanText();
    list.append("--report-sma");
    list.append(t);

    t = ui->StepPeriod->cleanText();
    list.append("--step-period");
    list.append(t);

    t = ui->StepLength->cleanText();
    list.append("--step-length");
    list.append(t);

    t = ui->StepDW->cleanText();
    list.append("--step-detection-window");
    list.append(t);

    t = ui->StepDT->cleanText();
    list.append("--step-detection-threshold");
    list.append(t);

    t = ui->SigDW->cleanText();
    list.append("--signal-detection-window");
    list.append(t);

    t = ui->SigDT->cleanText();
    list.append("--signal-detection-threshold");
    list.append(t);

    t = ui->GliDW->cleanText();
    list.append("--glitch-detection-window");
    list.append(t);

    t = ui->GliDT->cleanText();
    list.append("--glitch-detection-threshold");
    list.append(t);

    return list;
}

void MainWindow::update_latency(LatencyResult latency)
{
    ui->HwSwLatency->setText(QString::number(latency.swHw, 'f', 3) + QString("ms"));
    ui->SwLatency->setText(QString::number(latency.hw, 'f', 3)+ QString("ms"));
    ui->HwAvg5Latency->setText(QString::number(latency.hwAvg5, 'f', 3)+ QString("ms"));
}

} // namespace signal_estimator
