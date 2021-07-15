#pragma once
#include <QVector>
#include <QProcess>
#include <memory>
#include <cstring>
#include <QPointF>
#include <QFileInfo>
#include <QPointer>



bool checkSignalEstimator();

QVector<QString> getOutputDevices();
QVector<QString> getInputDevices();

QPointer<QProcess> startSignalEstimator(QStringList args);

QPointF parseLine(QString buffer);

QString formatDeviceName(QString);


