// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "SignalEstimator.hpp"

#include "core/Sample.hpp"
#include "core/Time.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>

#include <exception>

namespace signal_estimator {

namespace {

std::optional<LatencyResult> parseLatency(const QString& buffer) {
    LatencyResult values;
    QRegularExpression reg("([\\d\\.]+)ms");
    QStringList list;
    QRegularExpressionMatchIterator i = reg.globalMatch(buffer);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        list << match.captured(1);
    }

    if (list.size() != 3) {
        return {};
    }

    values.swHw = list[0].toDouble();
    values.hw = list[1].toDouble();
    values.hwAvgN = list[2].toDouble();
    return values;
}

std::optional<LossesResult> parseLosses(const QString& buffer) {
    LossesResult values;
    QRegularExpression reg("([\\d\\.]+)/sec|([\\d\\.]+)%");
    QStringList list;
    QRegularExpressionMatchIterator i = reg.globalMatch(buffer);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        list << match.captured(1);
    }

    if (list.size() != 3) {
        return {};
    }

    values.rate = list[0].toDouble();
    values.avgRate = list[1].toDouble();
    values.ratio = list[2].toDouble();
    return values;
}

std::optional<SignalPoint> parseSignal(const QString& buffer) {
    QRegularExpression reg(",|\\n");
    QStringList tokens = buffer.split(reg, Qt::SkipEmptyParts);
    if (tokens.count() < 3) {
        return {};
    }

    SignalPoint pt;

    pt.isOutput = tokens[0] == "o";
    try {
        pt.time = tokens[1].toDouble();
    } catch (const std::invalid_argument&) {
        return {};
    }
    try {
        pt.amplitude = tokens[2].toDouble();
    } catch (const std::invalid_argument&) {
        return {};
    }

    return pt;
}

} // namespace

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

    startTime_ = wallclock_timestamp_ns();

    return true;
}

void SignalEstimator::stop() {
    if (proc_ && proc_->isOpen()) {
        proc_->close();
    }

    proc_ = {};
}

std::optional<DataPoint> SignalEstimator::read() {
    if (!proc_) {
        return {};
    }

    if (!proc_->canReadLine()) {
        return {};
    }

    QByteArray buffer = proc_->readLine();

    return parseIO_(QString(buffer));
}

void SignalEstimator::clearResults_() {
    latency_ = {};
    losses_ = {};
}

std::optional<DataPoint> SignalEstimator::parseIO_(const QString& buffer) {
    if (buffer[0] == "#") {
        return {};
    }

    DataPoint pt {};

    if (buffer.size() > 1 && buffer[0] == "l" && buffer[1] == "a") {
        if (auto latencyValues = parseLatency(buffer)) {
            clearResults_();
            latency_ = *latencyValues;
            pt.type = PointType::Latency;
            pt.time = (wallclock_timestamp_ns() - startTime_) / Millisecond;
            pt.data1 = latency_->hwAvgN;
            pt.data2 = latency_->hw;
            pt.data3 = latency_->swHw;
            return pt;
        }
    }

    if (buffer.size() > 1 && buffer[0] == "l" && buffer[1] == "o") {
        if (auto lossesValues = parseLosses(buffer)) {
            clearResults_();
            losses_ = *lossesValues;
            pt.type = PointType::Losses;
            pt.time = (wallclock_timestamp_ns() - startTime_) / Millisecond;
            pt.data1 = losses_->rate;
            pt.data2 = losses_->avgRate;
            pt.data3 = losses_->ratio;
            return pt;
        }
    }

    if (buffer.size() > 1 && (buffer[0] == "i" || buffer[0] == "o")) {
        if (auto sigPoint = parseSignal(buffer)) {
            pt.type = sigPoint->isOutput ? PointType::Output : PointType::Input;
            pt.time = (sigPoint->time - startTime_) / Millisecond;
            pt.data1 = sigPoint->amplitude / MaxSample * 100;
            return pt;
        }
    }

    return {};
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
