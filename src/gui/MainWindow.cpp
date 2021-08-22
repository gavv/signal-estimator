#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    if (!check_signal_estimator()) { // open error window if signal-estimator is not found
        SigEstNotFound w;
        w.exec();
        return;
    }
    ui->setupUi(this);

    curve2_->attach(this->ui->OutputSig); // output curve
    curve1_->attach(this->ui->OutputSig); // input curve
    curve1_->setPen(QColor(Qt::GlobalColor(Qt::blue))); // set input curve color to blue
    curve2_->setPen(QColor(Qt::GlobalColor(Qt::red))); // set input curve color to red

    QVector<QString> in = get_input_devices();
    QVector<QString> out = get_output_devices();

    this->ui->InputDevices->addItems(in.toList()); // add input devices to combobox
    this->ui->OutputDevices->addItems(out.toList()); // add output devices to combobox

    this->show();
    return;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_graphs() { // update both input and output signal graphs
    QVector<QPointF> in_current = in_data_.get_current_points();

    QVector<QPointF> out_current = out_data_.get_current_points();

    this->curve1_->setSamples(in_current);
    this->curve2_->setSamples(out_current);

    this->ui->OutputSig->updateAxes();
    this->ui->OutputSig->replot();
    QApplication::processEvents();

    if (this->update_plots_ == true) { // restart timer for graphing
        this->timer_->start();
    }
}

void MainWindow::read_graph_data() {
    QString buffer;
    std::tuple<QPointF, IOType> pt;
    QByteArray arr;
    while (this->proc_->canReadLine()) {
        arr = this->proc_->readLine(); // read line from proc
        buffer = QString(arr); // convert to QString
        pt = parse_line(buffer);
        if (std::get<1>(pt) == Input) {
            this->in_data_.append_point(std::get<0>(pt));
        } else if (std::get<1>(pt) == Output) {
            this->out_data_.append_point(std::get<0>(pt));
        }
    }
}

QStringList MainWindow::set_up_program() {
    // This program calls the separate signal_estimator cmd line program to get the graph
    // data
    QStringList list; // to convert from QString to normal string
    QString temp;
    std::string buffer;

    list.append(" -m");
    list.append(this->ui->Modes->currentText());

    temp = this->ui->OutputDevices->currentText();
    // there might be a problem when there is too much arguments.
    // the arguments start to corrupt when there is too many of them
    // so for a lot of options we only add them as arguments when they are not default
    // value
    list.append("-o");
    list.append(format_device_name(temp));

    temp = this->ui->InputDevices->currentText();
    list.append("-i");
    list.append(format_device_name(temp));

    QString t;
    t = this->ui->SampleRate->cleanText();
    list.append("-r");
    list.append((t));

    t = this->ui->NumChannels->cleanText();
    list.append("-c");
    list.append(t);

    t = this->ui->SignalVolume->cleanText();
    list.append("-v");
    list.append(t);

    t = this->ui->PRB->cleanText();
    list.append("-p");
    list.append(t);

    t = this->ui->RBS->cleanText();
    list.append("-l");
    list.append(t);

    t = this->ui->Duration->cleanText();
    list.append("-d");
    list.append(t);

    // both of these options have to be stdout because we use a pipe that reads from
    // signal_estimator stdout
    list << "--dump-output"
         << "-";
    list << "--dump-input"
         << "-";

    t = this->ui->SMA->cleanText();

    list.append("--sma");
    list.append(t);

    // list.append("-f");

    // list.append("text");

    t = this->ui->StrikePeriod->cleanText();
    list.append("--strike-period");
    list.append(t);

    t = this->ui->StrikeLength->cleanText();
    list.append("--strike-length");
    list.append(t);

    t = this->ui->StrDW->cleanText();
    list.append("--strike-detection-window");
    list.append(t);

    t = this->ui->StrDT->cleanText();
    list.append("--strike-detection-threshold");
    list.append(t);

    t = this->ui->SigDW->cleanText();
    list.append("--signal-detection-window");
    list.append(t);

    t = this->ui->SigDT->cleanText();
    list.append("--signal-detection-threshold");
    list.append(t);

    t = this->ui->GliDW->cleanText();
    list.append("--glitch-detection-window");
    list.append(t);

    t = this->ui->GliDT->cleanText();
    list.append("--glitch-detection-threshold");
    list.append(t);

    t = this->ui->MVFD->cleanText();
    list.append("--dump-frame");
    list.append(t);

    t = this->ui->DR->cleanText();
    list.append("--dump-rounding");
    list.append(t);

    return list;
}

void MainWindow::run_estimator() {
    QStringList args = this->set_up_program();

    this->set_update_plots(true); // must be true to update graphs

    // clear old data when the start button is pressed
    in_data_.clear_buf();
    out_data_.clear_buf();

    // reset graphs
    this->ui->OutputSig->updateAxes();
    this->ui->OutputSig->replot();

    this->proc_ = start_signal_estimator(args);
    QStringList argslist = this->proc_->arguments();
    // proc emits error signal
    this->proc_->connect(this->proc_.data(), SIGNAL(error(QProcess::ProcessError)), this,
        SLOT(check_proc()));

    QTimer* time = new QTimer(0);

    this->timer_ = time; // timer for ui graphing

    timer_->setInterval(20);
    timer_->connect(timer_, SIGNAL(timeout()), this, SLOT(update_graphs()));
    this->proc_->connect(this->proc_.data(), SIGNAL(readyReadStandardOutput()), this,
        SLOT(read_graph_data()));
    // this->proc_->connect(this->proc,SIGNAL(finished(int)), this,
    // SLOT(on_StopButton_clicked()));

    if (this->proc_->open(QProcess::ReadOnly) == false) // failing? to open signal-est
    {
        this->ui->ErrorLabel->setText(QString("Failed to open signal-estimator"));
    }
    this->set_update_plots(true);
    this->timer_->start();

    return;
}

void MainWindow::on_StartButton_released() {
    this->run_estimator();
}

void MainWindow::on_StopButton_clicked() {
    if (this->proc_->isOpen()) {
        this->proc_->close();
    } // close QProcess

    this->set_update_plots(false); // stop plotting when stop button is pressed
}

void MainWindow::check_proc() { // get string proc for error
    this->ui->ErrorLabel->setText(this->proc_->errorString());
}
