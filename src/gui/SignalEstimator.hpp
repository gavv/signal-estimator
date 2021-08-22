#pragma once

#include "PointType.hpp"

#include <QPointF>
#include <QPointer>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QVector>

#include <tuple>

bool check_signal_estimator();

QSharedPointer<QProcess> start_signal_estimator(QStringList args);

std::tuple<QPointF, PointType> parse_line(QString buffer);
