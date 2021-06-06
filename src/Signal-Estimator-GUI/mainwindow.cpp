#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    char buffer[128];

    curve2->attach(this->ui->OutputSig);
    curve1->attach(this->ui->InputSig);

    // run pipe to get output devices from aplay
    std::unique_ptr<FILE,decltype(&pclose)> pipe1(popen("aplay -l","r"),pclose);

    if(!pipe){
        throw std::runtime_error("Failed to run aplay");
    }

    // read everything from aplay into result
    while (fgets(buffer,128,pipe1.get()) != nullptr){
        if (std::strstr(buffer,"card") != NULL && std::strstr(buffer,"device") != NULL) // if line has both card and device in it
            this->ui->OutputDevices->addItem(QString::fromStdString(buffer)); // add to combobox
    }

    std::unique_ptr<FILE,decltype(&pclose)> pipe2(popen("arecord -l","r"),pclose);
    if(!pipe2){
        throw std::runtime_error("Failed to run arecord");
    }

    while (fgets(buffer,128,pipe2.get()) != nullptr){
        if (std::strstr(buffer,"card") != NULL && std::strstr(buffer,"device") != NULL) // if line has both card and device in it
            this->ui->InputDevices->addItem(QString::fromStdString(buffer)); // add to combobox
    }
    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_graphs(){ // update both input and output signal graphs
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

     // run pipe with signal_estimator
     // This assumes that the signal_estimator is outputting to stdout with the type of signal prepended to the data values
     // The only changes to the signal_estimator source are the fact that frame.hpp now has a function to get iotype
     // and the file dumper dumps the data prepended with iotype to stdout if stdout is used as file dumper option.

     std::unique_ptr<FILE,decltype(&pclose)> pipe2(popen(cmd,"r"),pclose);
     if(!pipe2){
         throw std::runtime_error("Failed to run signal_estimator");
     }
     // loop to get a line from the pipe

     while (fgets(buffer,128,pipe2.get()) != nullptr && this->get_update_plots() == true){

        token = std::strtok(buffer," \n"); // get iotype
        m.lock();
        // if iotype is input
        if (strcmp(token, "Input") == 0){
            token = std::strtok(NULL, " \n");
            inx.append(std::stod(token)/1000000); // append to Qvector x-axis for input graph
            token = std::strtok(NULL, " \n");
            if (token == NULL) // if corresponding y cannot be read, toss back the x out
                inx.pop_back();
            else // append to Qvector y-axis for input graph
                iny.append(std::stod(token));
            if (counter % 20 == 0){ // set the data for QWT plot curve in the input graph
                incurve->setSamples(inx, iny);
            }
        }
        // if iotype is output
        else if (strcmp(token, "Output") == 0) {
            token = std::strtok(NULL, " \n");
            outx.append(std::stod(token)/1000000); // append to Qvector x-axis for output graph
            token = std::strtok(NULL, " \n");
            if (token == NULL) // if corresponding y cannot be read, toss back the x out
                outx.pop_back();
            else // append to Qvector y-axis for input graph
                outy.append(std::stod(token));
            if (counter % 20 == 0){ // set the data for QWT plot curve in the output graph
                outcurve->setSamples(outx, outy);
            }
        }
        m.unlock();

        if (counter == 5000)
            counter = 0;

     }
     this->set_update_plots(false); // after read loop finishes set to false to stop reading graph
}

void MainWindow::run_estimator(){
        std::string options; // used to hold all options from the ui
        // This program calls the separate signal_estimator cmd line program to get the graph data
        QString temp; // to convert from QString to normal string

        temp = this->ui->Format->currentText();

        options.append(" -f ");
        options.append(temp.toStdString());

        temp = this->ui->Modes->currentText();

        options.append(" -m ");
        options.append(temp.toStdString());

        char c;

        temp = this->ui->OutputDevices->currentText();
        options.append(" -o ");
        if (temp.toStdString() == "default")
            options.append(temp.toStdString());
        else {

            c = *(std::strstr(temp.toStdString().c_str() , "card ") + 5); // get card number
            options.append("hw:"); // hw:
            options.push_back(c); // hw:X
            c = *(std::strstr(temp.toStdString().c_str() , " device ") + 8); // get device
            options.append(","); // hw:X,
            options.push_back(c); // hw:X,Y
        }

        temp = this->ui->InputDevices->currentText();
        options.append(" -i ");
        if (temp.toStdString() == "default")
            options.append(temp.toStdString());
        else {

            c = *(std::strstr(temp.toStdString().c_str() , "card ") + 5);
            options.append("hw:");
            options.push_back(c);
            c = *(std::strstr(temp.toStdString().c_str() , " device ") + 8);
            options.append(",");
            options.push_back(c);
        }

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

        // both of these options have to be stdout because we use a pipe that reads from signal_estimator stdout
        options.append(" --dump-output stdout ");
        options.append(" --dump-input stdout ");

        options.append(this->eo->get_options());
        this->eo->wipe_options();

        std::string command = "./signal-estimator ";
        command.append(options);
        const char* cmd = command.data(); //

        this->set_update_plots(true); // must be true to update graphs

        // clear old data when the start button is pressed
        Inx.clear();
        Iny.clear();
        Outx.clear();
        Outy.clear();

        // reset graphs
        this->ui->OutputSig->updateAxes();
        this->ui->OutputSig->replot();

        std::mutex m; // needed to make sure the thread reading in data and the thread graphing the data don't step on each other's toes

        // Thread that reads in data
        std::thread t1(&MainWindow::read_graph_data, this,cmd,
                       std::ref(curve1),std::ref(curve2),std::ref(Inx),std::ref(Iny),
                       std::ref(Outx),std::ref(Outy),std::ref(m));

        while (this->get_update_plots() == true){ // loop to update graph (must be done in main GUI thread)
            usleep(.01 * 1000000);
            m.lock();
            this->update_graphs();
            m.unlock();
        }

        t1.join();

        return;
}

void MainWindow::on_MoreOptionsButton_released()
{
    this->eo->show(); // show more options window
}

void MainWindow::on_StartButton_released()
{
    this->run_estimator();
}

void MainWindow::on_StopButton_clicked()
{
    this->set_update_plots(false); // stop plotting when stop button is pressed
}
