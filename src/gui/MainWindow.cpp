// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "RightClickPickerMachine.hpp"

#include <QCheckBox>

#include <qwt_legend.h>
#include <qwt_picker_machine.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

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

    connect(ui->GridCheckBox, &QCheckBox::stateChanged,
        [this](int state) {
            const bool enable = (state == Qt::Checked);
            grid_->enableX(enable);
            grid_->enableY(enable);
            ui->OutputSig->replot();
        });

    zoomInPicker_ = new QwtPlotPicker(ui->OutputSig->canvas());
    zoomInPicker_->setStateMachine(new QwtPickerDragRectMachine());
    zoomInPicker_->setRubberBand(QwtPicker::RectRubberBand);
    zoomInPicker_->setRubberBandPen(QColor(Qt::red));
    connect(zoomInPicker_, qOverload<const QRectF&>(&QwtPlotPicker::selected),
        [this](const QRectF& rect) {
            ui->OutputSig->setAxisScale(
                QwtPlot::xBottom, rect.topLeft().x(), rect.bottomRight().x());
            ui->OutputSig->setAxisScale(
                QwtPlot::yLeft, rect.topLeft().y(), rect.bottomLeft().y());
            ui->OutputSig->replot();
        });

    zoomOutPicker_ = new QwtPlotPicker(ui->OutputSig->canvas());
    zoomOutPicker_->setStateMachine(new RightClickPickerMachine());
    connect(zoomOutPicker_, qOverload<const QPointF&>(&QwtPlotPicker::selected),
        [this](const QPointF&) {
            ui->OutputSig->setAxisAutoScale(QwtPlot::xBottom);
            ui->OutputSig->setAxisAutoScale(QwtPlot::yLeft);
            ui->OutputSig->replot();
        });


    trackPicker_ = new QwtPlotPicker(ui->OutputSig->canvas());
    trackPicker_->setStateMachine(new QwtPickerTrackerMachine());
    connect(trackPicker_, &QwtPlotPicker::moved,
        [this](const QPointF& pos) {
            ui->CursorPositionLabel->setText(
                QString("(%1, %2)")
                    .arg(QString::number(pos.x()))
                    .arg(QString::number(pos.y())));
        });

    QVector<QString> in = get_input_devices();
    QVector<QString> out = get_output_devices();

    ui->InputDevices->addItems(in.toList()); // add input devices to combobox
    ui->OutputDevices->addItems(out.toList()); // add output devices to combobox

    show();
}

MainWindow::~MainWindow() {
    if (proc_ && proc_->isOpen()) {
        proc_->close();
    }

    delete ui;
}

void MainWindow::update_graphs() {
    if (!update_plots_) {
        return;
    }

    // update both input and output signal graphs
    QVector<QPointF> in_current = in_data_.get_current_points();
    QVector<QPointF> out_current = out_data_.get_current_points();

    auto xMax = std::max(in_current.back().x(), out_current.back().x());
    auto xMin = xMax - out_data_.size_milliseconds();

    inputCurve_->setSamples(in_current);
    outputCurve_->setSamples(out_current);

    ui->OutputSig->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    ui->OutputSig->replot();
}

void MainWindow::read_graph_data() {
    QString buffer;
    std::tuple<QPointF, PointType> pt;
    QByteArray arr;
    while (proc_->canReadLine()) {
        arr = proc_->readLine(); // read line from proc
        buffer = QString(arr); // convert to QString
        pt = parse_line(buffer);
        if (std::get<1>(pt) == PointType::Input) {
            in_data_.append_point(std::get<0>(pt));
        } else if (std::get<1>(pt) == PointType::Output) {
            out_data_.append_point(std::get<0>(pt));
        }
    }
}

QStringList MainWindow::set_up_program_() {
    // This program calls the separate signal_estimator cmd line program to get the graph
    // data
    QStringList list; // to convert from QString to normal string
    QString temp;
    std::string buffer;

    list.append(" -m");
    list.append(ui->Modes->currentText());

    temp = ui->OutputDevices->currentText();
    // there might be a problem when there is too much arguments.
    // the arguments start to corrupt when there is too many of them
    // so for a lot of options we only add them as arguments when they are not default
    // value
    list.append("-o");
    list.append(format_device_name(temp));

    temp = ui->InputDevices->currentText();
    list.append("-i");
    list.append(format_device_name(temp));

    QString t;
    t = ui->SampleRate->cleanText();
    list.append("-r");
    list.append((t));

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

    // both of these options have to be stdout because we use a pipe that reads from
    // signal_estimator stdout
    list << "--dump-output"
         << "-";
    list << "--dump-input"
         << "-";

    t = ui->SMA->cleanText();

    list.append("--sma");
    list.append(t);

    t = ui->StrikePeriod->cleanText();
    list.append("--strike-period");
    list.append(t);

    t = ui->StrikeLength->cleanText();
    list.append("--strike-length");
    list.append(t);

    t = ui->StrDW->cleanText();
    list.append("--strike-detection-window");
    list.append(t);

    t = ui->StrDT->cleanText();
    list.append("--strike-detection-threshold");
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

    t = ui->MVFD->cleanText();
    list.append("--dump-frame");
    list.append(t);

    t = ui->DR->cleanText();
    list.append("--dump-rounding");
    list.append(t);

    return list;
}

void MainWindow::run_estimator_() {
    QStringList args = set_up_program_();

    set_update_plots_(true); // must be true to update graphs

    // clear old data when the start button is pressed
    in_data_.clear_buf();
    out_data_.clear_buf();

    // reset graphs
    ui->OutputSig->updateAxes();
    ui->OutputSig->replot();

    if (proc_ && proc_->isOpen()) {
        proc_->close();
    }
    proc_ = start_signal_estimator(args);

    // proc emits error signal
    proc_->connect(proc_.data(),
        qOverload<QProcess::ProcessError>(&QProcess::errorOccurred), this,
        &MainWindow::check_proc);

    if (!timer_) {
        timer_ = new QTimer(this);
        timer_->setInterval(20);
        timer_->connect(timer_, &QTimer::timeout, this, &MainWindow::update_graphs);
    }

    proc_->connect(proc_.data(), &QProcess::readyReadStandardOutput, this,
        &MainWindow::read_graph_data);

    // failing to open signal-estimator?
    if (proc_->open(QProcess::ReadOnly)) {
        ui->ErrorLabel->setText("");
    } else {
        ui->ErrorLabel->setText(QString("Failed to open signal-estimator"));
    }
    set_update_plots_(true);
    timer_->start();
}

void MainWindow::on_StartButton_released() {
    run_estimator_();
}

void MainWindow::on_StopButton_clicked() {
    if (proc_->isOpen()) {
        proc_->close(); // close QProcess
    }

    set_update_plots_(false); // stop plotting when stop button is pressed
}

void MainWindow::check_proc() {
    // get string proc for error
    ui->ErrorLabel->setText(proc_->errorString());
}
