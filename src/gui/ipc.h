#pragma once
#include <QVector>
#include <QProcess>
#include <QPointF>
#include <QFileInfo>
#include <QPointer>

#include <tuple>
#include <memory>
#include <cstring>
#include "points.h"

bool checkSignalEstimator();

QVector<QString> getOutputDevices();
QVector<QString> getInputDevices();

QSharedPointer<QProcess> startSignalEstimator(QStringList args);

std::tuple<QPointF, IOType> parseLine(QString buffer);

QString formatDeviceName(QString);


