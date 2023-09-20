// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "SignalEstimator.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <QStandardPaths>

#include <exception>

namespace signal_estimator {

SignalEstimator::SignalEstimator(QObject* parent)
    : QObject(parent) {
}

SignalEstimator::~SignalEstimator() {
    stop();
}

QString SignalEstimator::find() {
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

bool SignalEstimator::start(QStringList args) {
    stop();

    proc_ = QSharedPointer<QProcess>(new QProcess);

    proc_->setProcessChannelMode(QProcess::MergedChannels);
    proc_->setProgram(find());
    proc_->setArguments(args);
    proc_->setReadChannel(QProcess::StandardOutput);

    connect(proc_.data(), &QProcess::readyReadStandardOutput, this,
        &SignalEstimator::can_read);

    connect(proc_.data(), qOverload<QProcess::ProcessError>(&QProcess::errorOccurred),
        this, [this]() {
            if (proc_) {
                error(proc_->errorString());
            }
        });

    if (!proc_->open(QProcess::ReadOnly)) {
        proc_ = {};
        return false;
    }

    return true;
}

void SignalEstimator::stop() {
    if (proc_ && proc_->isOpen()) {
        proc_->close();
    }

    proc_ = {};
}

std::optional<std::tuple<QPointF, PointType>> SignalEstimator::read() {
    if (!proc_) {
        return {};
    }

    if (!proc_->canReadLine()) {
        return {};
    }

    QByteArray buffer = proc_->readLine();

    return parse_(QString(buffer));
}

std::optional<std::tuple<QPointF, PointType>> SignalEstimator::parse_(QString buffer) {
    if (buffer[0] == "#") {
        return {};
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
        return {};
    }

    QPointF pt;

    try {
        pt.setX(tokens[1].toDouble() / 1000000);
    } catch (const std::invalid_argument&) {
        return {};
    }

    try {
        pt.setY(tokens[2].toDouble() / 1000);
    } catch (const std::invalid_argument&) {
        return {};
    }

    if (tokens[0] == "i") {
        return std::make_tuple(pt, PointType::Input);
    }

    if (tokens[0] == "o") {
        return std::make_tuple(pt, PointType::Output);
    }

    return {};
}

} // namespace signal_estimator
