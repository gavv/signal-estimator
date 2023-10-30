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
#include <qwt_symbol.h>

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

    outputCurve_->setPen(QColor(0x1f77b4));
    outputCurve_->attach(ui->OutputSig);

    inputCurve_->setPen(QColor(0xff7f0e));
    inputCurve_->attach(ui->OutputSig);

    {
        data1Curve_->setPen(QColor(0x107F7B), 2);

        QwtSymbol* symbol1 = new QwtSymbol(QwtSymbol::Ellipse);
        symbol1->setColor(data1Curve_->pen().color());
        symbol1->setSize(5);

        data1Curve_->setSymbol(symbol1);
        data1Curve_->setLegendAttribute(QwtPlotCurve::LegendShowLine);
        data1Curve_->attach(ui->ResultPlot1);
    }

    {
        data2Curve_->setPen(QColor(0x14A3A1), 2);

        QColor color2 = data2Curve_->pen().color();
        color2.setAlphaF(0.1);

        QwtSymbol* symbol2 = new QwtSymbol(QwtSymbol::Ellipse);
        symbol2->setColor(data2Curve_->pen().color());
        symbol2->setSize(5);

        data2Curve_->setBrush(QBrush(color2));
        data2Curve_->setSymbol(symbol2);
        data2Curve_->setLegendAttribute(QwtPlotCurve::LegendShowLine);
        data2Curve_->attach(ui->ResultPlot1);
    }

    {
        data3Curve_->setPen(QColor(0xB2A3B5), 2);

        QColor color3 = data3Curve_->pen().color();
        color3.setAlphaF(0.1);

        QwtSymbol* symbol3 = new QwtSymbol(QwtSymbol::Ellipse);
        symbol3->setColor(data3Curve_->pen().color());
        symbol3->setSize(5);

        data3Curve_->setBrush(QBrush(color3));
        data3Curve_->setSymbol(symbol3);
        data3Curve_->setLegendAttribute(QwtPlotCurve::LegendShowLine);
        data3Curve_->attach(ui->ResultPlot2);
    }

    ui->OutputSig->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    ui->ResultPlot1->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    ui->ResultPlot2->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

    grid1_->setMajorPen(Qt::black, 0.0, Qt::DotLine);
    grid1_->attach(ui->OutputSig);
    grid2_->setMajorPen(Qt::black, 0.0, Qt::DotLine);
    grid2_->attach(ui->ResultPlot1);
    grid3_->setMajorPen(Qt::black, 0.0, Qt::DotLine);
    grid3_->attach(ui->ResultPlot2);

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

    QwtPlotPicker* trackPicker1 = new QwtPlotPicker(ui->OutputSig->canvas());
    trackPicker1->setStateMachine(new QwtPickerTrackerMachine());
    trackPicker1->setTrackerPen(QApplication::palette().text().color());
    trackPicker1->setTrackerMode(QwtPlotPicker::DisplayMode::AlwaysOn);

    QwtPlotPicker* trackPicker2 = new QwtPlotPicker(ui->ResultPlot1->canvas());
    trackPicker2->setStateMachine(new QwtPickerTrackerMachine());
    trackPicker2->setTrackerPen(QApplication::palette().text().color());
    trackPicker2->setTrackerMode(QwtPlotPicker::DisplayMode::AlwaysOn);

    QwtPlotPicker* trackPicker3 = new QwtPlotPicker(ui->ResultPlot2->canvas());
    trackPicker3->setStateMachine(new QwtPickerTrackerMachine());
    trackPicker3->setTrackerPen(QApplication::palette().text().color());
    trackPicker3->setTrackerMode(QwtPlotPicker::DisplayMode::AlwaysOn);

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

    display_latency_text_();

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
    data1_.clear_buf();
    data2_.clear_buf();
    data3_.clear_buf();

    // reset graphs
    ui->OutputSig->updateAxes();
    ui->OutputSig->replot();
    ui->ResultPlot1->updateAxes();
    ui->ResultPlot1->replot();
    ui->ResultPlot2->updateAxes();
    ui->ResultPlot2->replot();

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
    clear_results_text_();
    timer_->start();
}

void MainWindow::on_StopButton_clicked() {
    signal_estimator_->stop();

    set_update_plots_(false);
}

void MainWindow::update_graphs() {
    if (auto latencyValues = signal_estimator_->latencyUpdate()) {
        update_latency_(*latencyValues);
    }
    if (auto lossesValues = signal_estimator_->lossesUpdate()) {
        update_losses_(*lossesValues);
    }

    if (!update_plots_) {
        return;
    }
    // update all graphs
    QVector<QPointF> in_current = in_data_.get_current_points();
    QVector<QPointF> out_current = out_data_.get_current_points();
    QVector<QPointF> data1_current = data1_.get_current_points();
    QVector<QPointF> data2_current = data2_.get_current_points();
    QVector<QPointF> data3_current = data3_.get_current_points();

    if (in_current.isEmpty() || out_current.isEmpty()) {
        return;
    }

    auto xMax = std::max(in_current.back().x(), out_current.back().x());
    auto xMin = xMax - out_data_.size_milliseconds();

    inputCurve_->setSamples(in_current);
    outputCurve_->setSamples(out_current);
    data1Curve_->setSamples(data1_current);
    data2Curve_->setSamples(data2_current);
    data3Curve_->setSamples(data3_current);

    ui->OutputSig->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    ui->OutputSig->replot();
    ui->ResultPlot1->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    ui->ResultPlot1->replot();
    ui->ResultPlot2->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    ui->ResultPlot2->replot();
}

void MainWindow::read_graph_data() {
    while (auto entry = signal_estimator_->read()) {
        auto pt = *entry;

        if (pt.type == PointType::Input) {
            in_data_.append_point({ pt.time, pt.data1 });
        } else if (pt.type == PointType::Output) {
            out_data_.append_point({ pt.time, pt.data1 });
        } else if (pt.type == PointType::Latency || pt.type == PointType::Losses) {
            data1_.append_point({ pt.time, pt.data1 });
            data2_.append_point({ pt.time, pt.data2 });
            data3_.append_point({ pt.time, pt.data3 });
        }
    }
}

void MainWindow::show_error(QString error) {
    ui->ErrorLabel->setText(error);
}

QStringList MainWindow::set_up_program_() {
    QStringList list;
    QString t;

    list.append("--mode");
    list.append(ui->Modes->currentText());

    t = ui->OutputDevices->currentText();
    list.append("--output");
    list.append(
        QString::fromStdString(device_manager_.format_device_name(t.toStdString())));

    t = ui->InputDevices->currentText();
    list.append("--input");
    list.append(
        QString::fromStdString(device_manager_.format_device_name(t.toStdString())));

    t = ui->SampleRate->cleanText();
    list.append("--rate");
    list.append(t);

    t = ui->NumChannels->cleanText();
    list.append("--chans");
    list.append(t);

    t = ui->SignalVolume->cleanText();
    list.append("--gain");
    list.append(t);

    t = ui->PRB->cleanText();
    list.append("--out-periods");
    list.append(t);

    t = ui->RBS->cleanText();
    list.append("--out-latency");
    list.append(t);

    t = ui->Duration->cleanText();
    list.append("--duration");
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
    list.append("--step-interval");
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

void MainWindow::set_update_plots_(bool f) {
    update_plots_ = f;
}

void MainWindow::update_latency_(LatencyResult latency) {
    display_latency_text_();
    ui->Result1->setText(QString::number(latency.swHw, 'f', 3) + QString("ms"));
    ui->Result2->setText(QString::number(latency.hw, 'f', 3) + QString("ms"));
    ui->Result3->setText(QString::number(latency.hwAvgN, 'f', 3) + QString("ms"));
}

void MainWindow::update_losses_(LossesResult latency) {
    display_losses_text_();
    ui->Result1->setText(QString::number(latency.rate, 'f', 3) + QString("/sec"));
    ui->Result2->setText(QString::number(latency.avgRate, 'f', 3) + QString("/sec"));
    ui->Result3->setText(QString::number(latency.ratio, 'f', 3) + QString("%"));
}

void MainWindow::display_latency_text_() {
    ui->ResultLabel1->setText("Hardware + Software Latency:");
    ui->ResultLabel2->setText("Hardware Latency:");
    ui->ResultLabel3->setText("Average Hardware Latency:");
    data1Curve_->setTitle("Average Hardware Latency");
    data2Curve_->setTitle("Hardware Latency");
    data3Curve_->setTitle("Hardware + Software Latency");
}

void MainWindow::display_losses_text_() {
    ui->ResultLabel1->setText("Loss Rate:");
    ui->ResultLabel2->setText("Average Loss Rate:");
    ui->ResultLabel3->setText("Loss Ratio:");
    data1Curve_->setTitle("Loss Rate");
    data2Curve_->setTitle("Average Loss Rate");
    data3Curve_->setTitle("Loss Ratio");
}

void MainWindow::clear_results_text_() {
    ui->ResultLabel1->setText("");
    ui->ResultLabel2->setText("");
    ui->ResultLabel3->setText("");
    ui->Result1->setText("");
    ui->Result2->setText("");
    ui->Result3->setText("");
}

} // namespace signal_estimator
