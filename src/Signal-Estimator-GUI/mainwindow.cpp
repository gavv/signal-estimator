#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    char buffer[128];
    size_t pos = 100, end;
    std::string device_name;
    std::string result;

    std::unique_ptr<FILE,decltype(&pclose)> pipe1(popen("aplay -l","r"),pclose);
    if(!pipe){
        throw std::runtime_error("Failed to run aplay");
    }

    while (fgets(buffer,128,pipe1.get()) != nullptr){
        result += buffer;
    }

    int done = 0;

    while (done != 1 ){
        pos = result.find(" device ") + 11; // try to find " device " in string and set start at device name start
        result = result.substr(pos);
        end = result.find(" Subdevice"); // get position of end of device name
        if (end > 0 && pos > 11){ // if end is shorter than result length and string::find found device
            device_name = result.substr(0,end - 2); // get device name
            this->ui->OutputDevices->addItem(QString::fromStdString(device_name)); // add to combobox
        }
        else
            done = 1; // to exit loop
    }

    std::unique_ptr<FILE,decltype(&pclose)> pipe2(popen("arecord -l","r"),pclose);
    if(!pipe2){
        throw std::runtime_error("Failed to run aplay");
    }
    result = "";
    while (fgets(buffer,128,pipe2.get()) != nullptr){
        result += buffer;
    }
    done = 0;
    pos = 100; // reset for input device loop

    while (done != 1){ // same loop as getting output devices but for input devices
        pos = result.find(" device ") + 11;
        result = result.substr(pos);
        end = result.find(" Subdevice");
        if (end > 0 && pos > 11){
            device_name = result.substr(0,end - 2);

            this->ui->InputDevices->addItem(QString::fromStdString(device_name));
        }
        else
            done = 1;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::read_graph_data(FILE* file, QwtPlotCurve* curve, QVector<double> x, QVector<double> y){
     char buffer[128];
     std::string token;
     int counter = 0;
     while (fgets(buffer,128,file) != nullptr){
        token = std::strtok(buffer," \n");
        x.append(std::stod(token)/1000000);
        token = std::strtok(NULL, " \n");
        y.append(std::stod(token));

        //token = std::strtok(buffer," \n\0");
        //token = std::strtok(NULL, " \n\0");

        counter += 1;

        if (counter == 20){
            curve->setSamples(x, y);
            counter = 0;
            usleep(.2 * 1000000) ;
            if (curve->title().text().toStdString() == "Input Curve"){
                curve->attach(this->ui->InputSig);
                this->ui->InputSig->updateAxes();
                this->ui->InputSig->replot();
            }
            else {
                curve->attach(this->ui->OutputSig);
                this->ui->OutputSig->updateAxes();
                this->ui->OutputSig->replot();
            }
            QCoreApplication::processEvents();

        }

     }
}

void MainWindow::run_estimator(){
        std::string options;
        QString temp;

        temp = this->ui->Format->currentText();

        options.append(" -f ");
        options.append(temp.toStdString());

        temp = this->ui->Modes->currentText();

        options.append(" -m ");
        options.append(temp.toStdString());

        temp = this->ui->OutputDevices->currentText();

        options.append(" -o ");
        options.append(temp.toStdString());

        temp = this->ui->InputDevices->currentText();

        options.append(" -i ");
        options.append(temp.toStdString());

        int t;
        t = this->ui->SampleRate->value();
        options.append(" -r ");
        options.append(std::to_string(t));

        t =  this->ui->NumChannels->value();
        options.append(" -c ");
        options.append(std::to_string(t));

        t =  this->ui->SignalVolume->value();
        options.append(" -v ");
        options.append(std::to_string(t));

        t = this->ui->PRB->value();
        options.append(" -p ");
        options.append(std::to_string(t));

        t = this->ui->RBS->value();
        options.append(" -l ");
        options.append(std::to_string(t));

        t = this->ui->Duration->value();
        options.append(" -d ");
        options.append(std::to_string(t));

        options.append(" --dump-output tempout.txt ");

        options.append(" --dump-input tempin.txt ");

        options.append(this->eo->get_options());
        this->eo->wipe_options();

        std::string command = "./signal-estimator ";
        command.append(options);


        const char* cmd = command.data();

        system(cmd);

        char* token ; // need to differentiate between input signal and output
        QVector<double> Outx;
        QVector<double> Outy;
        QVector<double> Inx;
        QVector<double> Iny;

        QwtPlotCurve* curve1 = new QwtPlotCurve ("Input Curve");
        QwtPlotCurve* curve2 = new QwtPlotCurve ("Output Curve");

        FILE* infp = fopen("tempin.txt", "r");
        FILE* outfp = fopen("tempout.txt", "r");

        this->read_graph_data(infp,curve1,Inx,Iny);
        std::fclose(infp);
        this->read_graph_data(outfp,curve2,Outx,Outy);
        std::fclose(outfp);

        return;
}

void MainWindow::on_MoreOptionsButton_released()
{
    this->eo->show();
}

void MainWindow::on_StartButton_released()
{
    QFuture<void> future = QtConcurrent::run(this->run_estimator());
}
