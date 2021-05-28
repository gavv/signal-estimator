#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "extra_options.h"
#include <memory>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <qwt_plot_curve.h>
#include <qwt_text.h>
#include <cstring>
#include <unistd.h>
#include <QtConcurrent/QtConcurrent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void run_estimator();
    void read_graph_data(FILE* file, QwtPlotCurve* curve, QVector<double> x, QVector<double> y);

private slots:
    void on_MoreOptionsButton_released();

    void on_StartButton_released();

private:
    Ui::MainWindow *ui;
    extra_options* eo = new extra_options(this);

};

#endif // MAINWINDOW_H
