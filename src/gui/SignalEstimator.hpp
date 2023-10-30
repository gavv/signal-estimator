// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "PointType.hpp"
#include "core/Time.hpp"

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

struct SignalPoint {
    bool isOutput { false };
    double time { 0 };
    double amplitude { 0 };
};

struct DataPoint {
    PointType type {};
    double time {};
    // Depending on PointType:
    double data1 {}; // Avg HW Latency   | Loss Rate     | Signal level
    double data2 {}; // HW  Latency      | Avg Loss Rate | Not used (garbage)
    double data3 {}; // HW + SW Latency  | Loss Ratio    | Not used (garbage)
};

class SignalEstimator : public QObject {
    Q_OBJECT

public:
    static QString find();

    SignalEstimator(QObject* parent = nullptr);
    ~SignalEstimator() override;

    bool start(QStringList args);
    void stop();

    std::optional<DataPoint> read();
    std::optional<LatencyResult> latencyUpdate();
    std::optional<LossesResult> lossesUpdate();

signals:
    void can_read();
    void error(QString);

private:
    std::optional<DataPoint> parseIO_(const QString& buffer);
    void clearResults_();

    QSharedPointer<QProcess> proc_;
    std::optional<LatencyResult> latency_ = {};
    std::optional<LossesResult> losses_ = {};

    nanoseconds_t startTime_ {};
};

} // namespace signal_estimator
