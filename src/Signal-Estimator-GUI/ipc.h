#pragma once
#include <QVector>
#include <QProcess>
#include <memory>
#include <cstring>
#include <QPointF>
#include <QFileInfo>



bool checkSignalEstimator();

QVector<QString> getOutputDevices();
QVector<QString> getInputDevices();

QProcess* startSignalEstimator(QStringList args);

QPointF parseLine(QString buffer);

QString formatDeviceName(QString);


