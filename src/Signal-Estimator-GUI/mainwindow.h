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
#include <QTimer>
#include <thread>
#include <mutex>


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
    void read_graph_data(const char* cmd, QwtPlotCurve* incurve, QwtPlotCurve* outcurve,
                                     QVector<double>& inx, QVector<double>& iny,
                                     QVector<double>& outx, QVector<double>& outy, std::mutex& m);

private slots:
    void on_MoreOptionsButton_released();

    void on_StartButton_released();
    void set_update_plots(bool f){this->update_plots = f;}
    bool get_update_plots(){return this->update_plots;}
    void update_graphs();
    void on_StopButton_clicked();

private:
    Ui::MainWindow *ui;
    extra_options* eo = new extra_options(this);
    bool update_plots = false;
    QVector<double> Outx;
    QVector<double> Outy;
    QVector<double> Inx;
    QVector<double> Iny;

    QwtPlotCurve* curve1 = new QwtPlotCurve ("Input Curve");
    QwtPlotCurve* curve2 = new QwtPlotCurve ("Output Curve");
signals:
    void update_input_sig() const;
    void update_output_sig() const;
};



#endif // MAINWINDOW_H
