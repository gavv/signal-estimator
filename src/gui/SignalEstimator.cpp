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
    if (buffer[0] == "#") {
        return std::make_tuple(QPointF {}, PointType::None);
    }

    QRegExp reg;
    reg.setPattern(QString(",|\\n"));

    QStringList tokens = buffer.split(reg,
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        Qt::SkipEmptyParts
#else
        QString::SkipEmptyParts
#endif
    );
    if (tokens.count() < 3) {
        return std::make_tuple(QPointF {}, PointType::None);
    }

    QPointF pt;

    try {
        pt.setX(tokens[1].toDouble() / 1000000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
    }

    try {
        pt.setY(tokens[2].toDouble() / 1000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
        pt.setY(0.0);
    }

    if (!pt.isNull() && tokens[0] == "i") {
        return std::make_tuple(pt, PointType::Input);
    }

    if (!pt.isNull() && tokens[0] == "o") {
        return std::make_tuple(pt, PointType::Output);
    }

    return std::make_tuple(QPointF {}, PointType::None);
}
