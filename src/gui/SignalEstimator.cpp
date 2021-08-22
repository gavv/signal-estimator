#include "SignalEstimator.hpp"

#include <QFileInfo>
#include <QRegExp>

#include <exception>

bool check_signal_estimator() {
    // check signal-estimator exists in current dir
    if (QFileInfo(QString("signal-estimator")).isExecutable()) {
        return true;
    } else {
        return false;
    }
}

QSharedPointer<QProcess> start_signal_estimator(QStringList args) {
    // setup qprocess for signal-estimator
    QSharedPointer<QProcess> proc = QSharedPointer<QProcess>(new QProcess);
    QString command = "./signal-estimator";
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->setProgram(command);
    proc->setArguments(args);
    proc->setReadChannel(QProcess::StandardOutput);
    return proc;
}

std::tuple<QPointF, PointType> parse_line(QString buffer) {
    QPointF pt;
    QRegExp reg;
    reg.setPattern(QString("\\s+"));
    std::tuple<QPointF, PointType> pt_info;

    QStringList token = buffer.split(reg, QString::SkipEmptyParts);
    if (token.count() != 3) {
        pt_info = std::make_tuple(pt, PointType::None);
        return pt_info;
    }
    try {
        pt.setX(token[1].toDouble() / 1000000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
    }

    try {
        pt.setY(token[2].toDouble() / 1000);
    } catch (const std::invalid_argument&) {
        pt.setX(0.0);
        pt.setY(0.0);
    }

    if (pt.isNull() == false && buffer.contains(QString("in"))) {
        pt_info = std::make_tuple(pt, PointType::Input);
    } else if (pt.isNull() == false && buffer.contains(QString("out"))) {
        pt_info = std::make_tuple(pt, PointType::Output);
    } else {
        pt_info = std::make_tuple(pt, PointType::None);
    }

    return pt_info;
}
