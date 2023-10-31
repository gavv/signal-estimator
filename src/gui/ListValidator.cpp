// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "ListValidator.hpp"

namespace signal_estimator {

ListValidator::ListValidator(const QStringList& allowed_values, QObject* parent)
    : QValidator(parent)
    , allowed_values_(allowed_values) {
}

QValidator::State ListValidator::validate(QString& input, int&) const {
    if (allowed_values_.contains(input, Qt::CaseInsensitive)) {
        return Acceptable;
    }

    for (const auto& value : allowed_values_) {
        if (value.startsWith(input, Qt::CaseInsensitive)) {
            return Intermediate;
        }
    }

    return Invalid;
}

} // namespace signal_estimator
