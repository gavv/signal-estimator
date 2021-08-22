#pragma once

#include "IOType.hpp"

#include <QFileInfo>
#include <QPointF>
#include <QPointer>
#include <QProcess>
#include <QVector>

#include <cstring>
#include <memory>
#include <tuple>

bool check_signal_estimator();

QVector<QString> get_output_devices();
QVector<QString> get_input_devices();

QSharedPointer<QProcess> start_signal_estimator(QStringList args);

std::tuple<QPointF, IOType> parse_line(QString buffer);

QString format_device_name(QString);
