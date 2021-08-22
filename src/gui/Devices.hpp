#pragma once

#include <QString>
#include <QVector>

QVector<QString> get_output_devices();
QVector<QString> get_input_devices();

QString format_device_name(QString);
