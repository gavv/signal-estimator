// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <QString>
#include <QVector>

class DeviceManager {
public:
    DeviceManager() = default;

    QVector<QString> get_output_devices();
    QVector<QString> get_input_devices();

    QString format_device_name(QString);
};
