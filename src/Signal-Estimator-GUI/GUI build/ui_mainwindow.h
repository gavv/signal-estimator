/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QComboBox *InputDevices;
    QComboBox *OutputDevices;
    QComboBox *Modes;
    QSpinBox *SampleRate;
    QLabel *SampleRateLabel;
    QPushButton *StartButton;
    QPushButton *StopButton;
    QComboBox *Format;
    QSpinBox *NumChannels;
    QLabel *NumChannelsLabel;
    QLabel *VolumeLabel_2;
    QSpinBox *SignalVolume;
    QLabel *PRBLabel;
    QSpinBox *PRB;
    QSpinBox *RBS;
    QLabel *RBSLabel;
    QSpinBox *Duration;
    QLabel *DurationLabel;
    QLabel *InputLabel;
    QLabel *OutputLabel;
    QwtPlot *InputSig;
    QwtPlot *OutputSig;
    QPushButton *MoreOptionsButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 800);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        InputDevices = new QComboBox(centralWidget);
        InputDevices->setObjectName(QStringLiteral("InputDevices"));
        InputDevices->setGeometry(QRect(10, 10, 171, 41));
        OutputDevices = new QComboBox(centralWidget);
        OutputDevices->setObjectName(QStringLiteral("OutputDevices"));
        OutputDevices->setGeometry(QRect(190, 10, 191, 41));
        Modes = new QComboBox(centralWidget);
        Modes->setObjectName(QStringLiteral("Modes"));
        Modes->setGeometry(QRect(10, 60, 171, 41));
        SampleRate = new QSpinBox(centralWidget);
        SampleRate->setObjectName(QStringLiteral("SampleRate"));
        SampleRate->setGeometry(QRect(110, 110, 141, 26));
        SampleRate->setMinimum(1);
        SampleRate->setMaximum(2147483647);
        SampleRate->setValue(48000);
        SampleRateLabel = new QLabel(centralWidget);
        SampleRateLabel->setObjectName(QStringLiteral("SampleRateLabel"));
        SampleRateLabel->setGeometry(QRect(10, 110, 101, 21));
        StartButton = new QPushButton(centralWidget);
        StartButton->setObjectName(QStringLiteral("StartButton"));
        StartButton->setGeometry(QRect(20, 300, 89, 25));
        StopButton = new QPushButton(centralWidget);
        StopButton->setObjectName(QStringLiteral("StopButton"));
        StopButton->setGeometry(QRect(120, 300, 89, 25));
        Format = new QComboBox(centralWidget);
        Format->setObjectName(QStringLiteral("Format"));
        Format->setGeometry(QRect(190, 60, 191, 41));
        NumChannels = new QSpinBox(centralWidget);
        NumChannels->setObjectName(QStringLiteral("NumChannels"));
        NumChannels->setGeometry(QRect(160, 140, 91, 26));
        NumChannels->setMinimum(1);
        NumChannels->setMaximum(2147483647);
        NumChannels->setValue(2);
        NumChannelsLabel = new QLabel(centralWidget);
        NumChannelsLabel->setObjectName(QStringLiteral("NumChannelsLabel"));
        NumChannelsLabel->setGeometry(QRect(10, 140, 151, 21));
        VolumeLabel_2 = new QLabel(centralWidget);
        VolumeLabel_2->setObjectName(QStringLiteral("VolumeLabel_2"));
        VolumeLabel_2->setGeometry(QRect(10, 170, 111, 21));
        SignalVolume = new QSpinBox(centralWidget);
        SignalVolume->setObjectName(QStringLiteral("SignalVolume"));
        SignalVolume->setGeometry(QRect(130, 170, 121, 26));
        SignalVolume->setMinimum(0);
        SignalVolume->setMaximum(100);
        SignalVolume->setSingleStep(1);
        SignalVolume->setValue(50);
        PRBLabel = new QLabel(centralWidget);
        PRBLabel->setObjectName(QStringLiteral("PRBLabel"));
        PRBLabel->setGeometry(QRect(10, 200, 161, 21));
        PRB = new QSpinBox(centralWidget);
        PRB->setObjectName(QStringLiteral("PRB"));
        PRB->setGeometry(QRect(170, 200, 81, 26));
        PRB->setMinimum(0);
        PRB->setMaximum(2147483647);
        PRB->setSingleStep(1);
        PRB->setValue(2);
        RBS = new QSpinBox(centralWidget);
        RBS->setObjectName(QStringLiteral("RBS"));
        RBS->setGeometry(QRect(160, 230, 91, 26));
        RBS->setMinimum(0);
        RBS->setMaximum(2147483647);
        RBS->setSingleStep(1);
        RBS->setValue(8000);
        RBSLabel = new QLabel(centralWidget);
        RBSLabel->setObjectName(QStringLiteral("RBSLabel"));
        RBSLabel->setGeometry(QRect(10, 230, 151, 21));
        Duration = new QSpinBox(centralWidget);
        Duration->setObjectName(QStringLiteral("Duration"));
        Duration->setGeometry(QRect(250, 260, 91, 26));
        Duration->setMinimum(0);
        Duration->setMaximum(2147483647);
        Duration->setSingleStep(1);
        Duration->setValue(10);
        DurationLabel = new QLabel(centralWidget);
        DurationLabel->setObjectName(QStringLiteral("DurationLabel"));
        DurationLabel->setGeometry(QRect(10, 260, 231, 21));
        InputLabel = new QLabel(centralWidget);
        InputLabel->setObjectName(QStringLiteral("InputLabel"));
        InputLabel->setGeometry(QRect(10, 330, 101, 21));
        OutputLabel = new QLabel(centralWidget);
        OutputLabel->setObjectName(QStringLiteral("OutputLabel"));
        OutputLabel->setGeometry(QRect(10, 530, 101, 21));
        InputSig = new QwtPlot(centralWidget);
        InputSig->setObjectName(QStringLiteral("InputSig"));
        InputSig->setGeometry(QRect(10, 360, 371, 161));
        OutputSig = new QwtPlot(centralWidget);
        OutputSig->setObjectName(QStringLiteral("OutputSig"));
        OutputSig->setGeometry(QRect(10, 560, 371, 161));
        OutputSig->setAutoReplot(false);
        MoreOptionsButton = new QPushButton(centralWidget);
        MoreOptionsButton->setObjectName(QStringLiteral("MoreOptionsButton"));
        MoreOptionsButton->setGeometry(QRect(270, 110, 111, 51));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Signal Estimator", Q_NULLPTR));
        InputDevices->clear();
        InputDevices->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "default", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        InputDevices->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired input device.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        OutputDevices->clear();
        OutputDevices->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "default", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        OutputDevices->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired output device.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        Modes->clear();
        Modes->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "noop", Q_NULLPTR)
         << QApplication::translate("MainWindow", "latency", Q_NULLPTR)
         << QApplication::translate("MainWindow", "losses", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        Modes->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired mode</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        SampleRateLabel->setText(QApplication::translate("MainWindow", "Sample Rate:", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        StartButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Run signal-estimator and graph the results</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        StartButton->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        StopButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Stop graphing. Please wait for the sounds of signal-estimator to stop</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        StopButton->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
        Format->clear();
        Format->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "json", Q_NULLPTR)
         << QApplication::translate("MainWindow", "text", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        Format->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired format.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        NumChannelsLabel->setText(QApplication::translate("MainWindow", "Number of Channels:", Q_NULLPTR));
        VolumeLabel_2->setText(QApplication::translate("MainWindow", "Signal Volume:", Q_NULLPTR));
        PRBLabel->setText(QApplication::translate("MainWindow", "Periods in Ring Buffer:", Q_NULLPTR));
        RBSLabel->setText(QApplication::translate("MainWindow", "Ring Buffer Size (ms):", Q_NULLPTR));
        DurationLabel->setText(QApplication::translate("MainWindow", "Measurement Duration (seconds):", Q_NULLPTR));
        InputLabel->setText(QApplication::translate("MainWindow", "Input Signal:", Q_NULLPTR));
        OutputLabel->setText(QApplication::translate("MainWindow", "Output Signal:", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        MoreOptionsButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Open a window containing more options</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        MoreOptionsButton->setText(QApplication::translate("MainWindow", "Extra Options", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
