// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <QDialog>

namespace Ui {
class NotFoundDialog;
}

namespace signal_estimator {

class NotFoundDialog : public QDialog {
    Q_OBJECT

public:
    explicit NotFoundDialog(QWidget* parent = nullptr);
    ~NotFoundDialog() override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::NotFoundDialog* ui;
};

} // namespace signal_estimator
