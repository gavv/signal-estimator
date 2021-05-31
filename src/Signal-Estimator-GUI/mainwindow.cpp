#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect( this->update_input_sig, this, &MainWindow::update_input, Qt::QueuedConnection);
    //connect( this->update_output_sig, this, &MainWindow::update_output, Qt::QueuedConnection);

    char buffer[128];
    size_t pos = 100, end;
    std::string device_name;
    std::string result;

    curve2->attach(this->ui->OutputSig);
    curve1->attach(this->ui->InputSig);

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

void MainWindow::update_graphs(){
    //this->ui->InputSig->detachItems(QwtPlotItem::Rtti_PlotCurve,false);

    this->ui->InputSig->updateAxes();
    this->ui->InputSig->replot();

    //this->ui->OutputSig->detachItems(QwtPlotItem::Rtti_PlotCurve,false);

    this->ui->OutputSig->updateAxes();
    this->ui->OutputSig->replot();
    QApplication::processEvents();
}



void MainWindow::read_graph_data(const char* cmd, QwtPlotCurve* incurve, QwtPlotCurve* outcurve,
                                 QVector<double>& inx, QVector<double>& iny,
                                 QVector<double>& outx, QVector<double>& outy, std::mutex& m){
     char buffer[128];
     char* token;
     int counter = 0;

     std::unique_ptr<FILE,decltype(&pclose)> pipe2(popen(cmd,"r"),pclose);
     if(!pipe2){
         throw std::runtime_error("Failed to run signal_estimator");
     }

     while (fgets(buffer,128,pipe2.get()) != nullptr && this->get_update_plots() == true){

        token = std::strtok(buffer," \n");
        m.lock();
        if (strcmp(token, "Input") == 0){
            token = std::strtok(NULL, " \n");
            inx.append(std::stod(token)/1000000);
            token = std::strtok(NULL, " \n");
            if (token == NULL)
                inx.pop_back();
            else
                iny.append(std::stod(token));
            if (counter % 20 == 0){
                incurve->setSamples(inx, iny);

            }
        }
        else if (strcmp(token, "Output") == 0) {
            token = std::strtok(NULL, " \n");
            outx.append(std::stod(token)/1000000);
            token = std::strtok(NULL, " \n");
            if (token == NULL)
                outx.pop_back();
            else
                outy.append(std::stod(token));

            if (counter % 20 == 0){
                outcurve->setSamples(outx, outy);

            }
        }
        m.unlock();
        if (counter == 5000){
            //inx.clear();
            //iny.clear();
            //outx.clear();
            //outy.clear();
            counter = 0;
        }

        counter += 1;

     }
     this->set_update_plots(false);
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

        options.append(" --dump-output stdout ");

        options.append(" --dump-input stdout ");

        options.append(this->eo->get_options());
        this->eo->wipe_options();

        std::string command = "./signal-estimator ";
        command.append(options);
        const char* cmd = command.data();

        //system(cmd);
        this->set_update_plots(true);

        Inx.clear();
        Iny.clear();
        Outx.clear();
        Outy.clear();

        //this->ui->InputSig->detachItems(QwtPlotItem::Rtti_PlotCurve,false);
        //this->ui->OutputSig->detachItems(QwtPlotItem::Rtti_PlotCurve,false);
        this->ui->OutputSig->updateAxes();
        this->ui->OutputSig->replot();


        std::mutex m;

        std::thread t1(&MainWindow::read_graph_data, this,cmd,
                       std::ref(curve1),std::ref(curve2),std::ref(Inx),std::ref(Iny),
                       std::ref(Outx),std::ref(Outy),std::ref(m));

        while (this->get_update_plots() == true){
            usleep(.01 * 1000000);
            m.lock();
            this->update_graphs();
            m.unlock();
        }

        t1.join();

        //QThreadPool* pool = new QThreadPool;
        //QFuture<void> f1 = QtConcurrent::run(&MainWindow::read_graph_data,infp,curve1,Inx,Iny);
        //QFuture<void> f2 = QtConcurrent::run(&MainWindow::read_graph_data,outfp,curve2,Outx,Outy);
        //f1.waitForFinished();
        //f2.waitForFinished();

        return;
}

void MainWindow::on_MoreOptionsButton_released()
{
    this->eo->show();
}

void MainWindow::on_StartButton_released()
{
    this->run_estimator();
}

void MainWindow::on_StopButton_clicked()
{
    this->set_update_plots(false);
}
