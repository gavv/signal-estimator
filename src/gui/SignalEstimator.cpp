// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "SignalEstimator.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>

#include <exception>

namespace signal_estimator {

SignalEstimator::SignalEstimator(QObject* parent)
    : QObject(parent) 
    {
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

    return parseIO_(QString(buffer));
}

std::optional<std::tuple<QPointF, PointType>> SignalEstimator::parseIO_(QString buffer) {
    if (buffer[0] == "#") {
        return {};
    }

    if (buffer.size() > 1 && buffer[0] == "l" && buffer[1] == "a"){
        if(auto latencyValues = parseLatency_(buffer)){
            latency_ = *latencyValues;
        }
    }

    if (buffer.size() > 1 && buffer[0] == "l" && buffer[1] == "o"){
        if(auto lossesValues = parseLosses_(buffer)){
            losses_ = *lossesValues;
        }
    }

    QRegularExpression reg(",|\\n");

    QStringList tokens = buffer.split(reg, Qt::SkipEmptyParts);
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

std::optional<LatencyResult> SignalEstimator::parseLatency_(QString buffer) {
    LatencyResult values;
    QRegularExpression reg("([\\d\\.]+)ms");
    QStringList list;
    QRegularExpressionMatchIterator i = reg.globalMatch(buffer);
    while(i.hasNext()){
        QRegularExpressionMatch match = i.next();
        list << match.captured(1);
    }

    if (list.size() != 3){
        return {};
    }

    values.swHw = list[0].toDouble();
    values.hw = list[1].toDouble();
    values.hwAvgN = list[2].toDouble();
    return values;
}

std::optional<LossesResult> SignalEstimator::parseLosses_(QString buffer) {
    LossesResult values;
    QRegularExpression reg("([\\d\\.]+)/sec|([\\d\\.]+)%");
    QStringList list;
    QRegularExpressionMatchIterator i = reg.globalMatch(buffer);
    while(i.hasNext()){
        QRegularExpressionMatch match = i.next();
        list << match.captured(1);
    }

    if (list.size() != 3){
        return {};
    }

    values.rate = list[0].toDouble();
    values.avgRate = list[1].toDouble();
    values.ratio = list[2].toDouble();
    return values;
}

std::optional<LatencyResult> SignalEstimator::latencyUpdate() {
    std::optional<LatencyResult> latencyTmp = latency_;
    latency_ = {};
    return latencyTmp;
}

std::optional<LossesResult> SignalEstimator::lossesUpdate() {
    std::optional<LossesResult> lossesTmp = losses_;
    losses_ = {};
    return lossesTmp;
}


} // namespace signal_estimator
