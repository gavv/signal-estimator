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
    double hwAvg5;
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

signals:
    void can_read();
    void error(QString);

private:
    std::optional<LatencyResult> parseLatency_(QString buffer);
    std::optional<std::tuple<QPointF, PointType>> parse_(QString buffer);
    QSharedPointer<QProcess> proc_;
    LatencyResult latency_;
    bool latencyChanged_ = false;
};

} // namespace signal_estimator
