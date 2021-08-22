#include "Devices.hpp"

#include <cstdio>
#include <cstring>
#include <memory>

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
        // if line has both card and device in it
        if (std::strstr(buffer, "card") != NULL
            && std::strstr(buffer, "device") != NULL) {
            strvec.append(QString::fromStdString(buffer));
        }
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
        // if line has both card and device in it
        if (std::strstr(buffer, "card") != NULL
            && std::strstr(buffer, "device") != NULL) {
            strvec.append(QString::fromStdString(buffer));
        }
    }
    return strvec;
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
