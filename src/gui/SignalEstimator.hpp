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

class SignalEstimator : public QObject {
    Q_OBJECT

public:
    static QString find();

    SignalEstimator(QObject* parent = nullptr);
    ~SignalEstimator() override;

    bool start(QStringList args);
    void stop();

    std::optional<std::tuple<QPointF, PointType>> read();

signals:
    void can_read();
    void error(QString);

private:
    std::optional<std::tuple<QPointF, PointType>> parse_(QString buffer);

    QSharedPointer<QProcess> proc_;
};

} // namespace signal_estimator
