// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <QStringList>
#include <QValidator>

namespace signal_estimator {

class ListValidator : public QValidator {
    Q_OBJECT

public:
    explicit ListValidator(const QStringList& allowed_values, QObject* parent = nullptr);

    State validate(QString& input, int&) const override;

private:
    QStringList allowed_values_;
};

} // namespace signal_estimator
