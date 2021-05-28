#ifndef EXTRA_OPTIONS_H
#define EXTRA_OPTIONS_H

#include <QDialog>
#include <QString>
#include "../Config.hpp"
namespace Ui {
class extra_options;
}

class extra_options : public QDialog
{
    Q_OBJECT

public:
    explicit extra_options(QWidget *parent = 0);
    std::string get_options(){return this->options;}
    void wipe_options(){this->options = "";}

    ~extra_options();

private slots:
    void on_OkButton_released();

private:
    Ui::extra_options *ui;
    std::string options;
};

#endif // EXTRA_OPTIONS_H
