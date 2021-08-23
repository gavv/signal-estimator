// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "SignalEstimator.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <QStandardPaths>

#include <exception>

QString find_signal_estimator() {
    if (auto path = QDir::currentPath() + "/signal-estimator";
        QFileInfo(path).isExecutable()) {
        return path;
    }

    if (auto path = QCoreApplication::applicationDirPath() + "/signal-estimator";
        QFileInfo(path).isExecutable()) {
        return path;
    }

    if (auto path = QStandardPaths::findExecutable("signal-estimator");
        QFileInfo(path).isExecutable()) {
        return path;
    }

    return {};
}

QSharedPointer<QProcess> start_signal_estimator(QStringList args) {
    // setup qprocess for signal-estimator
    QSharedPointer<QProcess> proc = QSharedPointer<QProcess>(new QProcess);
    QString command = find_signal_estimator();
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->setProgram(command);
    proc->setArguments(args);
    proc->setReadChannel(QProcess::StandardOutput);
    return proc;
}

std::tuple<QPointF, PointType> parse_line(QString buffer) {
    QPointF pt;
    QRegExp reg;
    reg.setPattern(QString("\\s+"));
    std::tuple<QPointF, PointType> pt_info;

    QStringList token = buffer.split(reg,
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        Qt::SkipEmptyParts
#else
        QString::SkipEmptyParts
#endif
    );
    if (token.count() != 3) {
        pt_info = std::make_tuple(pt, PointType::None);
        return pt_info;
    }
    try {
        pt.setX(token[1].toDouble() / 1000000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
    }

    try {
        pt.setY(token[2].toDouble() / 1000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
        pt.setY(0.0);
    }

    if (pt.isNull() == false && buffer.contains(QString("in"))) {
        pt_info = std::make_tuple(pt, PointType::Input);
    } else if (pt.isNull() == false && buffer.contains(QString("out"))) {
        pt_info = std::make_tuple(pt, PointType::Output);
    } else {
        pt_info = std::make_tuple(pt, PointType::None);
    }

    return pt_info;
}
