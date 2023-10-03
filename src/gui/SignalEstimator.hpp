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

struct LatencyResult{
    double swHw;
    double hw;
    double hwAvgN;
};

struct LossesResult{
    double rate;
    double avgRate;
    double ratio;
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
    std::optional<LatencyResult> parseLatency_(QString buffer);
    std::optional<LossesResult> parseLosses_(QString buffer);
    std::optional<std::tuple<QPointF, PointType>> parseIO_(QString buffer);
    QSharedPointer<QProcess> proc_;
    std::optional<LatencyResult> latency_ = {};
    std::optional<LossesResult> losses_ = {};
};

} // namespace signal_estimator
