// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "PointType.hpp"

#include <QPointF>
#include <QProcess>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

#include <optional>
#include <tuple>

namespace signal_estimator {

struct LatencyResult {
    double swHw { 0 };
    double hw { 0 };
    double hwAvgN { 0 };
};

struct LossesResult {
    double rate { 0 };
    double avgRate { 0 };
    double ratio { 0 };
};

class SignalEstimator : public QObject {
    Q_OBJECT

public:
    static QString find();

    SignalEstimator(QObject* parent = nullptr);
    ~SignalEstimator() override;

    bool start(QStringList args);
    void stop();

    std::optional<std::tuple<QPointF, PointType>> read();
    std::optional<LatencyResult> latencyUpdate();
    std::optional<LossesResult> lossesUpdate();

signals:
    void can_read();
    void error(QString);

private:
    std::optional<std::tuple<QPointF, PointType>> parseIO_(const QString& buffer);
    void clearResults_();

    QSharedPointer<QProcess> proc_;
    std::optional<LatencyResult> latency_ = {};
    std::optional<LossesResult> losses_ = {};
};

} // namespace signal_estimator
