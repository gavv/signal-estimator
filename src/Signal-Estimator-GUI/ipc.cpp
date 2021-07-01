#include "ipc.h"

QVector<QString> getOutputDevices(){
    char buffer[128];
    QVector<QString> Strvec;
    // run pipe to get output devices from aplay
    std::unique_ptr<FILE,decltype(&pclose)> pipe1(popen("aplay -l","r"),pclose);

    if(!pipe1){
        throw std::runtime_error("Failed to run aplay");
    }

    // read everything from aplay into result
    while (fgets(buffer,128,pipe1.get()) != nullptr){
        if (std::strstr(buffer,"card") != NULL && std::strstr(buffer,"device") != NULL) // if line has both card and device in it
            Strvec.append(QString::fromStdString(buffer)); // add to combobox
    }
    return Strvec;
}
QVector<QString> getInputDevices(){
    char buffer[128];
    QVector<QString> Strvec;
    // run pipe to get output devices from arecord
    std::unique_ptr<FILE,decltype(&pclose)> pipe2(popen("arecord -l","r"),pclose);
    if(!pipe2){
        throw std::runtime_error("Failed to run arecord");
    }

    while (fgets(buffer,128,pipe2.get()) != nullptr){
        if (std::strstr(buffer,"card") != NULL && std::strstr(buffer,"device") != NULL) // if line has both card and device in it
            Strvec.append(QString::fromStdString(buffer)); // add to combobox
    }
    return Strvec;
}

QProcess* startSignalEstimator(QStringList args)
{
    // setup qprocess for signal-estimator
    QProcess* proc = new QProcess;
    QString command = "./signal-estimator";
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->setProgram(command);
    proc->setArguments(args);
    return proc;
}

bool checkSignalEstimator()
{
    if (FILE* check = fopen("signal-estimator","r")){ // check signal-estimator exists in current dir
        fclose(check);
        return true;
     }
    else
        return false;
}


