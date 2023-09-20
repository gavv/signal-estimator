// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "RightClickPickerMachine.hpp"

#include <qwt_event_pattern.h>

#include <QMouseEvent>

namespace signal_estimator {

RightClickPickerMachine::RightClickPickerMachine()
    : QwtPickerMachine(PointSelection) {
}

QList<QwtPickerMachine::Command> RightClickPickerMachine::transition(
    const QwtEventPattern& eventPattern, const QEvent* event) {
    QList<QwtPickerMachine::Command> cmdList;

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if (eventPattern.mouseMatch(
                QwtEventPattern::MouseSelect2, static_cast<const QMouseEvent*>(event))) {
            cmdList += Begin;
            cmdList += Append;
            cmdList += End;
        }
        break;
    }
    case QEvent::KeyPress: {
        const QKeyEvent* keyEvent = static_cast<const QKeyEvent*>(event);
        if (eventPattern.keyMatch(QwtEventPattern::KeySelect1, keyEvent)) {
            if (!keyEvent->isAutoRepeat()) {
                cmdList += Begin;
                cmdList += Append;
                cmdList += End;
            }
        }
        break;
    }
    default:
        break;
    }

    return cmdList;
}

} // namespace signal_estimator
