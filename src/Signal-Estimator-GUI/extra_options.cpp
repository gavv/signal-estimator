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

    this->hide();
}
