#include "ipc.h"

QVector<QString> getOutputDevices(){
    char buffer[128];
    QVector<QString> Strvec;
    // run pipe to get output devices from aplay
    std::unique_ptr<FILE,decltype(&pclose)> pipe1(popen("aplay -l","r"),pclose);

    if(!pipe1){
        return Strvec;
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
        return Strvec;
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
    proc->setReadChannel(QProcess::StandardOutput);
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

QPointF parseLine(QString buffer){

    QPointF pt;
    QRegExp reg;
    reg.setPattern(QString("\\s+"));

    QStringList token = buffer.split(reg, QString::SkipEmptyParts);
    if (token.count() != 3){return pt;}
    try {
       pt.setX(token[1].toDouble()/100000);
    } catch (const std::invalid_argument&){
       pt.setX(0.0);
       return pt;
    }

    try {
       pt.setY(token[2].toDouble()/1000);
    } catch (const std::invalid_argument&){
        pt.setX(0.0);
        pt.setY(0.0);
    }

    return pt;
}


QString formatDeviceName(QString buffer)
{
    char* c;
    if (buffer.toStdString() == "default")
        return buffer;
    else {
        QString temp = buffer;
        c = (std::strstr(temp.toStdString().c_str() , "card ") + 5); // get card number
        buffer = "hw:";
        buffer.push_back(*c); // hw:X
        c = (std::strstr(temp.toStdString().c_str() , " device ") + 8); // get device
        buffer.append(","); // hw:X,
        buffer.push_back(*c); // hw:X,Y
        return buffer;
    }
}
