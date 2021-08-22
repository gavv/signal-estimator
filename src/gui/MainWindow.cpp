#include "MainWindow.hpp"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    if (!checkSignalEstimator()){ // open error window if signal-estimator is not found
        SigEstNotFound w;
        w.exec();
        return;
    }
    ui->setupUi(this);

    curve2->attach(this->ui->OutputSig); // output curve
    curve1->attach(this->ui->OutputSig); // input curve
    curve1->setPen(QColor(Qt::GlobalColor(Qt::blue))); // set input curve color to blue
    curve2->setPen(QColor(Qt::GlobalColor(Qt::red))); // set input curve color to red

    QVector<QString> in = getInputDevices();
    QVector<QString> out = getOutputDevices();

    this->ui->InputDevices->addItems(in.toList()); // add input devices to combobox
    this->ui->OutputDevices->addItems(out.toList()); // add output devices to combobox

    this->show();
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_graphs(){ // update both input and output signal graphs
    QVector<QPointF> in_current = in_data.getCurrentPoints();

    QVector<QPointF> out_current = out_data.getCurrentPoints();

    //this->ui->ErrorLabel_2->setText(QString("updating graphs"));

    this->curve1->setSamples(in_current);
    this->curve2->setSamples(out_current);

   // this->ui->InputSig->updateAxes();
    //this->ui->InputSig->replot();

    this->ui->OutputSig->updateAxes();
    this->ui->OutputSig->replot();
    QApplication::processEvents();

    if (this->update_plots == true){ // restart timer for graphing
        this->timer->start();
    }
}



void MainWindow::read_graph_data(){
    QString buffer;
    std::tuple<QPointF, IOType> pt;
    QByteArray arr;
    while (this->proc->canReadLine()){
        arr = this->proc->readLine(); // read line from proc
        buffer = QString(arr); // convert to QString
        //this->ui->ErrorLabel_2->setText(QString(arr));
        pt = parseLine(buffer);
        if (std::get<1>(pt) == Input)
            this->in_data.appendPoint(std::get<0>(pt));
        else if (std::get<1>(pt) == Output)
            this->out_data.appendPoint(std::get<0>(pt));
    }
}

QStringList MainWindow::set_up_program(){
    // This program calls the separate signal_estimator cmd line program to get the graph data
    QStringList list; // to convert from QString to normal string
    QString temp;
    std::string buffer;

    list.append(" -m");
    list.append(this->ui->Modes->currentText());

    temp = this->ui->OutputDevices->currentText();
    // there might be a problem when there is too much arguments.
    // the arguments start to corrupt when there is too many of them
    // so for a lot of options we only add them as arguments when they are not default value
    list.append("-o");
    list.append(formatDeviceName(temp));

    temp = this->ui->InputDevices->currentText();
    list.append("-i");
    list.append(formatDeviceName(temp));

    QString t;
    t = this->ui->SampleRate->cleanText();
    list.append("-r");
    list.append((t));

    t =  this->ui->NumChannels->cleanText();
    list.append("-c");
    list.append(t);

    t =  this->ui->SignalVolume->cleanText();
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


    // both of these options have to be stdout because we use a pipe that reads from signal_estimator stdout
    list << "--dump-output" << "-";
    list << "--dump-input" << "-";

    t = this->ui->SMA->cleanText();

    list.append("--sma");
    list.append(t);

    //list.append("-f");

    //list.append("text");

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

void MainWindow::run_estimator(){
        QStringList args = this->set_up_program();

        this->set_update_plots(true); // must be true to update graphs

        // clear old data when the start button is pressed
        in_data.clearBuf();
        out_data.clearBuf();

        // reset graphs
        this->ui->OutputSig->updateAxes();
        this->ui->OutputSig->replot();

        this->proc = startSignalEstimator(args);
        QStringList argslist = this->proc->arguments();
        // proc emits error signal
        this->proc->connect(this->proc.data(),SIGNAL(error(QProcess::ProcessError)),this,SLOT(checkProc()));

        QTimer* time = new QTimer(0);

        this->timer = time; // timer for ui graphing

        timer->setInterval(20);
        timer->connect(timer, SIGNAL(timeout()),this,SLOT(update_graphs()));
        this->proc->connect(this->proc.data(),SIGNAL(readyReadStandardOutput()), this, SLOT(read_graph_data()));
        //this->proc->connect(this->proc,SIGNAL(finished(int)), this, SLOT(on_StopButton_clicked()));

        if (this->proc->open(QProcess::ReadOnly) == false) // failing? to open signal-est
        {this->ui->ErrorLabel->setText(QString("Failed to open signal-estimator"));}
        this->set_update_plots(true);
        this->timer->start();

        return;
}

void MainWindow::on_StartButton_released()
{
    this->run_estimator();
}

void MainWindow::on_StopButton_clicked()
{
    if (this->proc->isOpen()){this->proc->close();} // close QProcess

    this->set_update_plots(false); // stop plotting when stop button is pressed
}



void MainWindow::checkProc(){ // get string proc for error
    this->ui->ErrorLabel->setText(this->proc->errorString());
}
