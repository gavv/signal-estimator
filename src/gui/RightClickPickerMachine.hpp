// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <qwt_picker_machine.h>

class RightClickPickerMachine : public QwtPickerMachine {
public:
    RightClickPickerMachine();

    virtual QList<Command> transition(
        const QwtEventPattern& eventPattern, const QEvent* event);
};