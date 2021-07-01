#ifndef IPC_H
#define IPC_H
#include <QVector>
#include <QProcess>
#include <memory>
#include <cstring>



bool checkSignalEstimator();

QVector<QString> getOutputDevices();
QVector<QString> getInputDevices();

QProcess* startSignalEstimator(QStringList args);



#endif // IPC_H
