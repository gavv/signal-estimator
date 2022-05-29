// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <qwt_picker_machine.h>

class RightClickPickerMachine : public QwtPickerMachine {
public:
    RightClickPickerMachine();

    QList<Command> transition(
        const QwtEventPattern& eventPattern, const QEvent* event) override;
};
