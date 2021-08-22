#pragma once

#include "PointsBuffer.hpp"

#include <QFileInfo>
#include <QPointF>
#include <QPointer>
#include <QProcess>
#include <QVector>

#include <cstring>
#include <memory>
#include <tuple>

bool checkSignalEstimator();

QVector<QString> getOutputDevices();
QVector<QString> getInputDevices();

QSharedPointer<QProcess> startSignalEstimator(QStringList args);

std::tuple<QPointF, IOType> parseLine(QString buffer);

QString formatDeviceName(QString);
