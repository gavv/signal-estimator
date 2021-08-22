#pragma once

#include <QDialog>

namespace Ui {
class SigEstNotFound;
}

class SigEstNotFound : public QDialog {
    Q_OBJECT

public:
    explicit SigEstNotFound(QWidget* parent = 0);
    ~SigEstNotFound();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SigEstNotFound* ui;
};
