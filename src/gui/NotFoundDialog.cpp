// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "NotFoundDialog.hpp"
#include "ui_NotFoundDialog.h"

namespace signal_estimator {

NotFoundDialog::NotFoundDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::NotFoundDialog) {
    ui->setupUi(this);
}

NotFoundDialog::~NotFoundDialog() {
    delete ui;
}

void NotFoundDialog::on_pushButton_clicked() {
    close();
}

} // namespace signal_estimator
