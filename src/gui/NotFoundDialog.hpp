// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <QDialog>

namespace Ui {
class NotFoundDialog;
}

class NotFoundDialog : public QDialog {
    Q_OBJECT

public:
    explicit NotFoundDialog(QWidget* parent = 0);
    ~NotFoundDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::NotFoundDialog* ui;
};
