#include "extra_options.h"
#include "ui_extra_options.h"

extra_options::extra_options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::extra_options)
{
    ui->setupUi(this);
}

extra_options::~extra_options()
{
    delete ui;
}

void extra_options::on_OkButton_released()
{
    float t; // same as the mainwindow options
    // There is a function that returns this->options as a string in header file
    std::string options = this->options;

    t = this->ui->SMA->value();

    options = " -f ";

    options.append(std::to_string(t));

    t = this->ui->StrikePeriod->value();
    options.append(" --strike-period ");
    options.append(std::to_string(t));

    t = this->ui->StrikeLength->value();
    options.append(" --strike-length ");
    options.append(std::to_string(t));

    t = this->ui->StrDW->value();
    options.append(" --strike-detection-window ");
    options.append(std::to_string(t));

    t = this->ui->StrDT->value();
    options.append(" --strike-detection-threshold ");
    options.append(std::to_string(t));

    t = this->ui->SigDW->value();
    options.append(" --signal-detection-window ");
    options.append(std::to_string(t));

    t = this->ui->SigDT->value();
    options.append(" --signal-detection-threshold ");
    options.append(std::to_string(t));

    t = this->ui->GliDW->value();
    options.append(" --glitch-detection-window ");
    options.append(std::to_string(t));

    t = this->ui->GliDT->value() / 100;
    options.append(" --glitch-detection-threshold ");
    options.append(std::to_string(t));

    t = this->ui->MVFD->value();
    options.append(" --dump-frame ");
    options.append(std::to_string(t));

    t = this->ui->DR->value();
    options.append(" --dump-rounding ");
    options.append(std::to_string(t));

    this->hide();
}
