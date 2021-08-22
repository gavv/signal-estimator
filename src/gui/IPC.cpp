#include "IPC.hpp"

QVector<QString> get_output_devices() {
    char buffer[128];
    QVector<QString> strvec;
    // run pipe to get output devices from aplay
    std::unique_ptr<FILE, decltype(&pclose)> pipe1(popen("aplay -l", "r"), pclose);

    if (!pipe1) {
        return strvec;
    }

    // read everything from aplay into result
    while (fgets(buffer, sizeof(buffer), pipe1.get()) != nullptr) {
        if (std::strstr(buffer, "card") != NULL
            && std::strstr(buffer, "device") != NULL) // if line has both card and device
                                                      // in it
            strvec.append(QString::fromStdString(buffer)); // add to combobox
    }
    return strvec;
}

QVector<QString> get_input_devices() {
    char buffer[128];
    QVector<QString> strvec;
    // run pipe to get output devices from arecord
    std::unique_ptr<FILE, decltype(&pclose)> pipe2(popen("arecord -l", "r"), pclose);
    if (!pipe2) {
        return strvec;
    }

    while (fgets(buffer, sizeof(buffer), pipe2.get()) != nullptr) {
        if (std::strstr(buffer, "card") != NULL
            && std::strstr(buffer, "device") != NULL) // if line has both card and device
                                                      // in it
            strvec.append(QString::fromStdString(buffer)); // add to combobox
    }
    return strvec;
}

QSharedPointer<QProcess> start_signal_estimator(QStringList args) {
    // setup qprocess for signal-estimator
    QSharedPointer<QProcess> proc = QSharedPointer<QProcess>(new QProcess);
    QString command = "./signal-estimator";
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->setProgram(command);
    proc->setArguments(args);
    proc->setReadChannel(QProcess::StandardOutput);
    return proc;
}

bool check_signal_estimator() {
    if (QFileInfo(QString("signal-estimator")).isExecutable()) { // check signal-estimator
                                                                 // exists in current dir
        return true;
    } else
        return false;
}

std::tuple<QPointF, IOType> parse_line(QString buffer) {
    QPointF pt;
    QRegExp reg;
    reg.setPattern(QString("\\s+"));
    std::tuple<QPointF, IOType> pt_info;

    QStringList token = buffer.split(reg, QString::SkipEmptyParts);
    if (token.count() != 3) {
        pt_info = std::make_tuple(pt, None);
        return pt_info;
    }
    try {
        pt.setX(token[1].toDouble() / 100000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
    }

    try {
        pt.setY(token[2].toDouble() / 1000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
        pt.setY(0.0);
    }

    if (pt.isNull() == false && buffer.contains(QString("in")))
        pt_info = std::make_tuple(pt, Input);
    else if (pt.isNull() == false && buffer.contains(QString("out")))
        pt_info = std::make_tuple(pt, Output);
    else
        pt_info = std::make_tuple(pt, None);

    return pt_info;
}

QString format_device_name(QString buffer) {
    const char* c;
    if (buffer.toStdString() == "default")
        return buffer;
    else {
        QByteArray temp = buffer.toLocal8Bit();
        buffer = "hw:";
        c = (std::strstr(temp.data(), "card ") + 5); // get card number
        buffer.push_back(*c); // hw:X
        c = (std::strstr(temp.data(), " device ") + 8); // get device
        buffer.append(","); // hw:X,
        buffer.push_back(*c); // hw:X,Y
        return buffer;
    }
}
