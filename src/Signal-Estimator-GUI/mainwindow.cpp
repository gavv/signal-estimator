#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    // sort the data by x value so it doesn't jump randomly across graph
    std::sort(in_current.begin(),in_current.end(),QPFcompare());
    QVector<QPointF> out_current = out_data.getCurrentPoints();
    std::sort(out_current.begin(),out_current.end(),QPFcompare());
    this->ui->ErrorLabel_2->setText(QString("updating graphs"));

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
     std::string buffer;

     char* fullstr;
     QByteArray arr;

     this->proc->setReadChannel(QProcess::StandardOutput);
     arr = this->proc->readLine(); // read line from proc
     buffer = arr.toStdString(); // convert to std::string
     //this->ui->ErrorLabel_2->setText(QString(arr));

     fullstr = &buffer[0]; // convert to char* for strtok parsing;

     parseLine(fullstr);
}

QStringList MainWindow::set_up_program(){
    // This program calls the separate signal_estimator cmd line program to get the graph data
    QStringList list; // to convert from QString to normal string
    QString temp;
    std::string buffer;

    list.append(" -m");
    list.append(this->ui->Modes->currentText());

    char* c;

    temp = this->ui->OutputDevices->currentText();
    // there might be a problem when there is too much arguments.
    // the arguments start to corrupt when there is too many of them
    // so for a lot of options we only add them as arguments when they are not default value
    list.append("-o");
    if (temp.toStdString() == "default")
        list.append(temp);
    else {
        c = (std::strstr(temp.toStdString().c_str() , "card ") + 5); // get card number
        buffer = "hw:";
        buffer.push_back(*c); // hw:X
        c = (std::strstr(temp.toStdString().c_str() , " device ") + 8); // get device
        buffer.append(","); // hw:X,
        buffer.push_back(*c); // hw:X,Y
        list.append(QString::fromStdString(buffer));
    }

    temp = this->ui->InputDevices->currentText();
    list.append("-i");
    if (temp.toStdString() == "default")
        list.append(temp);
    else {
        c = (std::strstr(temp.toStdString().c_str() , "card ") + 5); // get card number
        buffer = "hw:";
        buffer.push_back(*c); // hw:X
        c = (std::strstr(temp.toStdString().c_str() , " device ") + 8); // get device
        buffer.append(","); // hw:X,
        buffer.push_back(*c); // hw:X,Y
        list.append(QString::fromStdString(buffer));
    }

    QString t;
    if ((t = this->ui->SampleRate->cleanText()) != QString::fromStdString("48000")){ // if it is not default value
        list.append("-r");
        list.append((t));
    }

    if ((t =  this->ui->NumChannels->cleanText()) != QString::fromStdString("2")){
        list.append("-c");
        list.append(t);
    }

    if ((t =  this->ui->SignalVolume->cleanText()) != QString::fromStdString("50")){
        list.append("-v");
        list.append(t);
    }

    if ((t = this->ui->PRB->cleanText()) != QString::fromStdString("2")){
        list.append("-p");
        list.append(t);
    }

    if ((t = this->ui->RBS->cleanText()) != QString::fromStdString("8000")){
        list.append("-l");
        list.append(t);
    }

    if ((t = this->ui->Duration->cleanText()) != QString::fromStdString("10")){
        list.append("-d");
        list.append(t);
    }

    // both of these options have to be stdout because we use a pipe that reads from signal_estimator stdout
    list << "--dump-output" << "stdout";
    list << "--dump-input" << "stdout";

    t = this->ui->SMA->cleanText();

    list.append("--sma");
    list.append(t);

    //list.append("-f");

    //list.append("text");

    if ((t = this->ui->StrikePeriod->cleanText()) != QString::fromStdString("1")){
        list.append("--strike-period");
        list.append(t);
    }

    if ((t = this->ui->StrikeLength->cleanText()) != QString::fromStdString("0.010000")){
        list.append("--strike-length");
        list.append(t);
    }

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

    if ((t = this->ui->MVFD->cleanText()) != QString::fromStdString("64")){
        list.append("--dump-frame");
        list.append(t);
    }

    if ((t = this->ui->DR->cleanText()) != QString::fromStdString("10")){
        list.append("--dump-rounding");
        list.append(t);
    }

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
        // proc emits error signal
        this->proc->connect(this->proc,SIGNAL(error(QProcess::ProcessError)),this,SLOT(checkProc()));

        QTimer* time = new QTimer(0);

        this->timer = time; // timer for ui graphing

        timer->setInterval(20);
        timer->connect(timer, SIGNAL(timeout()),this,SLOT(update_graphs()));
        this->proc->connect(this->proc,SIGNAL(readyReadStandardOutput()), this, SLOT(read_graph_data()));
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

void MainWindow::parseLine(char* buffer){
    // had to leave this function in mainwindow because we have to use QPointF for graphing
    // we could have this function outside of main window but we would need to emit signals
    // and the point buffers would need to be QObjects since connect() needs a receiver object
    //
    const char* token = std::strtok(buffer," \n"); // get iotype
    QPointF pt;
    io type;
    //this->ui->ErrorLabel->setText(QString("parseline"));
    if (strcmp(token,"Input"))
        type = Input;
    else if(strcmp(token,"Output"))
        type = Output;
    else
        return;

    token = strtok(NULL," \n");
    try {
       pt.setX(std::stod(token)/100000);
    } catch (const std::invalid_argument&){
        return;
    }

    token = strtok(NULL," \n");

    try {
        if (type == Input){
            pt.setY(std::stod(token)/1000);
        }
        else pt.setY(std::stod(token));
    } catch (const std::invalid_argument&){
        return ;
    }
    std::optional<QPointF> optpt{pt};
    if (optpt.has_value()){
        if (type == Input){
           in_data.appendPoint(pt);
        }
        else {
           out_data.appendPoint(pt);
        }
    }
    return;
}

void MainWindow::checkProc(){ // get string proc for error
    this->ui->ErrorLabel->setText(this->proc->errorString());
}
