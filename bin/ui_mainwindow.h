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
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *Tabs;
    QWidget *Signals;
    QFormLayout *formLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QFormLayout *formLayout_4;
    QGridLayout *gridLayout;
    QComboBox *OutputDevices;
    QLabel *label_3;
    QComboBox *Modes;
    QLabel *label_2;
    QLabel *label;
    QComboBox *InputDevices;
    QHBoxLayout *horizontalLayout;
    QPushButton *StopButton;
    QPushButton *StartButton;
    QLabel *OutputLabel;
    QwtPlot *OutputSig;
    QWidget *Options;
    QFormLayout *formLayout;
    QLabel *SampleRateLabel;
    QSpinBox *SampleRate;
    QLabel *NumChannelsLabel;
    QSpinBox *NumChannels;
    QLabel *VolumeLabel_2;
    QSpinBox *SignalVolume;
    QLabel *PRBLabel;
    QSpinBox *PRB;
    QLabel *RBSLabel;
    QSpinBox *RBS;
    QLabel *DurationLabel;
    QSpinBox *Duration;
    QLabel *Label1_6;
    QSpinBox *SigDW;
    QLabel *Label1_7;
    QDoubleSpinBox *SigDT;
    QLabel *Label1_8;
    QSpinBox *GliDW;
    QLabel *Label1_9;
    QDoubleSpinBox *GliDT;
    QLabel *Label1_10;
    QSpinBox *MVFD;
    QLabel *Label1_11;
    QSpinBox *DR;
    QLabel *Label1;
    QSpinBox *SMA;
    QLabel *Label1_2;
    QSpinBox *StrikePeriod;
    QLabel *Label1_3;
    QDoubleSpinBox *StrikeLength;
    QLabel *Label1_4;
    QSpinBox *StrDW;
    QLabel *Label1_5;
    QDoubleSpinBox *StrDT;
    QLabel *ErrorLabel;
    QLabel *ErrorLabel_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(600, 830);
        MainWindow->setMinimumSize(QSize(600, 800));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        Tabs = new QTabWidget(centralWidget);
        Tabs->setObjectName(QStringLiteral("Tabs"));
        Signals = new QWidget();
        Signals->setObjectName(QStringLiteral("Signals"));
        formLayout_2 = new QFormLayout(Signals);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        scrollArea = new QScrollArea(Signals);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setMinimumSize(QSize(0, 700));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 552, 698));
        formLayout_4 = new QFormLayout(scrollAreaWidgetContents_2);
        formLayout_4->setSpacing(6);
        formLayout_4->setContentsMargins(11, 11, 11, 11);
        formLayout_4->setObjectName(QStringLiteral("formLayout_4"));
        formLayout_4->setHorizontalSpacing(4);
        formLayout_4->setVerticalSpacing(4);
        formLayout_4->setContentsMargins(4, 4, 4, 4);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        OutputDevices = new QComboBox(scrollAreaWidgetContents_2);
        OutputDevices->setObjectName(QStringLiteral("OutputDevices"));

        gridLayout->addWidget(OutputDevices, 1, 1, 1, 1);

        label_3 = new QLabel(scrollAreaWidgetContents_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        Modes = new QComboBox(scrollAreaWidgetContents_2);
        Modes->setObjectName(QStringLiteral("Modes"));

        gridLayout->addWidget(Modes, 5, 1, 1, 1);

        label_2 = new QLabel(scrollAreaWidgetContents_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 5, 0, 1, 1);

        label = new QLabel(scrollAreaWidgetContents_2);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        InputDevices = new QComboBox(scrollAreaWidgetContents_2);
        InputDevices->setObjectName(QStringLiteral("InputDevices"));

        gridLayout->addWidget(InputDevices, 4, 1, 1, 1);


        formLayout_4->setLayout(0, QFormLayout::SpanningRole, gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        StopButton = new QPushButton(scrollAreaWidgetContents_2);
        StopButton->setObjectName(QStringLiteral("StopButton"));

        horizontalLayout->addWidget(StopButton);

        StartButton = new QPushButton(scrollAreaWidgetContents_2);
        StartButton->setObjectName(QStringLiteral("StartButton"));

        horizontalLayout->addWidget(StartButton);


        formLayout_4->setLayout(1, QFormLayout::FieldRole, horizontalLayout);

        OutputLabel = new QLabel(scrollAreaWidgetContents_2);
        OutputLabel->setObjectName(QStringLiteral("OutputLabel"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, OutputLabel);

        OutputSig = new QwtPlot(scrollAreaWidgetContents_2);
        OutputSig->setObjectName(QStringLiteral("OutputSig"));
        OutputSig->setAutoReplot(false);

        formLayout_4->setWidget(3, QFormLayout::SpanningRole, OutputSig);

        scrollArea->setWidget(scrollAreaWidgetContents_2);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, scrollArea);

        Tabs->addTab(Signals, QString());
        Options = new QWidget();
        Options->setObjectName(QStringLiteral("Options"));
        formLayout = new QFormLayout(Options);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        SampleRateLabel = new QLabel(Options);
        SampleRateLabel->setObjectName(QStringLiteral("SampleRateLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, SampleRateLabel);

        SampleRate = new QSpinBox(Options);
        SampleRate->setObjectName(QStringLiteral("SampleRate"));
        SampleRate->setMinimum(1);
        SampleRate->setMaximum(2147483647);
        SampleRate->setValue(48000);

        formLayout->setWidget(0, QFormLayout::FieldRole, SampleRate);

        NumChannelsLabel = new QLabel(Options);
        NumChannelsLabel->setObjectName(QStringLiteral("NumChannelsLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, NumChannelsLabel);

        NumChannels = new QSpinBox(Options);
        NumChannels->setObjectName(QStringLiteral("NumChannels"));
        NumChannels->setMinimum(1);
        NumChannels->setMaximum(2147483647);
        NumChannels->setValue(2);

        formLayout->setWidget(1, QFormLayout::FieldRole, NumChannels);

        VolumeLabel_2 = new QLabel(Options);
        VolumeLabel_2->setObjectName(QStringLiteral("VolumeLabel_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, VolumeLabel_2);

        SignalVolume = new QSpinBox(Options);
        SignalVolume->setObjectName(QStringLiteral("SignalVolume"));
        SignalVolume->setMinimum(0);
        SignalVolume->setMaximum(100);
        SignalVolume->setSingleStep(1);
        SignalVolume->setValue(50);

        formLayout->setWidget(2, QFormLayout::FieldRole, SignalVolume);

        PRBLabel = new QLabel(Options);
        PRBLabel->setObjectName(QStringLiteral("PRBLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, PRBLabel);

        PRB = new QSpinBox(Options);
        PRB->setObjectName(QStringLiteral("PRB"));
        PRB->setMinimum(0);
        PRB->setMaximum(2147483647);
        PRB->setSingleStep(1);
        PRB->setValue(2);

        formLayout->setWidget(3, QFormLayout::FieldRole, PRB);

        RBSLabel = new QLabel(Options);
        RBSLabel->setObjectName(QStringLiteral("RBSLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, RBSLabel);

        RBS = new QSpinBox(Options);
        RBS->setObjectName(QStringLiteral("RBS"));
        RBS->setMinimum(0);
        RBS->setMaximum(2147483647);
        RBS->setSingleStep(1);
        RBS->setValue(8000);

        formLayout->setWidget(4, QFormLayout::FieldRole, RBS);

        DurationLabel = new QLabel(Options);
        DurationLabel->setObjectName(QStringLiteral("DurationLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, DurationLabel);

        Duration = new QSpinBox(Options);
        Duration->setObjectName(QStringLiteral("Duration"));
        Duration->setMinimum(0);
        Duration->setMaximum(2147483647);
        Duration->setSingleStep(1);
        Duration->setValue(10);

        formLayout->setWidget(5, QFormLayout::FieldRole, Duration);

        Label1_6 = new QLabel(Options);
        Label1_6->setObjectName(QStringLiteral("Label1_6"));

        formLayout->setWidget(6, QFormLayout::LabelRole, Label1_6);

        SigDW = new QSpinBox(Options);
        SigDW->setObjectName(QStringLiteral("SigDW"));
        SigDW->setMaximum(2147483647);
        SigDW->setValue(48);

        formLayout->setWidget(6, QFormLayout::FieldRole, SigDW);

        Label1_7 = new QLabel(Options);
        Label1_7->setObjectName(QStringLiteral("Label1_7"));

        formLayout->setWidget(7, QFormLayout::LabelRole, Label1_7);

        SigDT = new QDoubleSpinBox(Options);
        SigDT->setObjectName(QStringLiteral("SigDT"));
        SigDT->setDecimals(6);
        SigDT->setMaximum(1);
        SigDT->setSingleStep(0.01);
        SigDT->setValue(0.15);

        formLayout->setWidget(7, QFormLayout::FieldRole, SigDT);

        Label1_8 = new QLabel(Options);
        Label1_8->setObjectName(QStringLiteral("Label1_8"));

        formLayout->setWidget(8, QFormLayout::LabelRole, Label1_8);

        GliDW = new QSpinBox(Options);
        GliDW->setObjectName(QStringLiteral("GliDW"));
        GliDW->setMaximum(2147483647);
        GliDW->setValue(32);

        formLayout->setWidget(8, QFormLayout::FieldRole, GliDW);

        Label1_9 = new QLabel(Options);
        Label1_9->setObjectName(QStringLiteral("Label1_9"));

        formLayout->setWidget(9, QFormLayout::LabelRole, Label1_9);

        GliDT = new QDoubleSpinBox(Options);
        GliDT->setObjectName(QStringLiteral("GliDT"));
        GliDT->setDecimals(6);
        GliDT->setMaximum(1);
        GliDT->setSingleStep(0.01);
        GliDT->setValue(0.05);

        formLayout->setWidget(9, QFormLayout::FieldRole, GliDT);

        Label1_10 = new QLabel(Options);
        Label1_10->setObjectName(QStringLiteral("Label1_10"));

        formLayout->setWidget(10, QFormLayout::LabelRole, Label1_10);

        MVFD = new QSpinBox(Options);
        MVFD->setObjectName(QStringLiteral("MVFD"));
        MVFD->setMaximum(2147483647);
        MVFD->setValue(64);

        formLayout->setWidget(10, QFormLayout::FieldRole, MVFD);

        Label1_11 = new QLabel(Options);
        Label1_11->setObjectName(QStringLiteral("Label1_11"));

        formLayout->setWidget(11, QFormLayout::LabelRole, Label1_11);

        DR = new QSpinBox(Options);
        DR->setObjectName(QStringLiteral("DR"));
        DR->setMaximum(2147483647);
        DR->setValue(10);

        formLayout->setWidget(11, QFormLayout::FieldRole, DR);

        Label1 = new QLabel(Options);
        Label1->setObjectName(QStringLiteral("Label1"));

        formLayout->setWidget(12, QFormLayout::LabelRole, Label1);

        SMA = new QSpinBox(Options);
        SMA->setObjectName(QStringLiteral("SMA"));
        SMA->setMaximum(2147483647);
        SMA->setValue(5);

        formLayout->setWidget(12, QFormLayout::FieldRole, SMA);

        Label1_2 = new QLabel(Options);
        Label1_2->setObjectName(QStringLiteral("Label1_2"));

        formLayout->setWidget(13, QFormLayout::LabelRole, Label1_2);

        StrikePeriod = new QSpinBox(Options);
        StrikePeriod->setObjectName(QStringLiteral("StrikePeriod"));
        StrikePeriod->setMaximum(2147483647);
        StrikePeriod->setValue(1);

        formLayout->setWidget(13, QFormLayout::FieldRole, StrikePeriod);

        Label1_3 = new QLabel(Options);
        Label1_3->setObjectName(QStringLiteral("Label1_3"));

        formLayout->setWidget(14, QFormLayout::LabelRole, Label1_3);

        StrikeLength = new QDoubleSpinBox(Options);
        StrikeLength->setObjectName(QStringLiteral("StrikeLength"));
        StrikeLength->setDecimals(6);
        StrikeLength->setMaximum(1);
        StrikeLength->setSingleStep(0.01);
        StrikeLength->setValue(0.01);

        formLayout->setWidget(14, QFormLayout::FieldRole, StrikeLength);

        Label1_4 = new QLabel(Options);
        Label1_4->setObjectName(QStringLiteral("Label1_4"));

        formLayout->setWidget(15, QFormLayout::LabelRole, Label1_4);

        StrDW = new QSpinBox(Options);
        StrDW->setObjectName(QStringLiteral("StrDW"));
        StrDW->setMinimum(0);
        StrDW->setMaximum(2147483647);
        StrDW->setValue(96);

        formLayout->setWidget(15, QFormLayout::FieldRole, StrDW);

        Label1_5 = new QLabel(Options);
        Label1_5->setObjectName(QStringLiteral("Label1_5"));

        formLayout->setWidget(16, QFormLayout::LabelRole, Label1_5);

        StrDT = new QDoubleSpinBox(Options);
        StrDT->setObjectName(QStringLiteral("StrDT"));
        StrDT->setDecimals(6);
        StrDT->setMaximum(1);
        StrDT->setSingleStep(0.01);
        StrDT->setValue(0.4);

        formLayout->setWidget(16, QFormLayout::FieldRole, StrDT);

        Tabs->addTab(Options, QString());

        verticalLayout_2->addWidget(Tabs);

        ErrorLabel = new QLabel(centralWidget);
        ErrorLabel->setObjectName(QStringLiteral("ErrorLabel"));

        verticalLayout_2->addWidget(ErrorLabel);

        ErrorLabel_2 = new QLabel(centralWidget);
        ErrorLabel_2->setObjectName(QStringLiteral("ErrorLabel_2"));

        verticalLayout_2->addWidget(ErrorLabel_2);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        Tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Signal Estimator", Q_NULLPTR));
        OutputDevices->clear();
        OutputDevices->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "default", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        OutputDevices->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired output device.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("MainWindow", "Input Device:", Q_NULLPTR));
        Modes->clear();
        Modes->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "noop", Q_NULLPTR)
         << QApplication::translate("MainWindow", "latency", Q_NULLPTR)
         << QApplication::translate("MainWindow", "losses", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        Modes->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired mode</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "Mode :", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Output Device :", Q_NULLPTR));
        InputDevices->clear();
        InputDevices->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "default", Q_NULLPTR)
        );
#ifndef QT_NO_TOOLTIP
        InputDevices->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Select the desired input device.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        StopButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Stop graphing. Please wait for the sounds of signal-estimator to stop</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        StopButton->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        StartButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Run signal-estimator and graph the results</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        StartButton->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
        OutputLabel->setText(QApplication::translate("MainWindow", "Output (Emitted) == red / Input (Captured) == blue", Q_NULLPTR));
        Tabs->setTabText(Tabs->indexOf(Signals), QApplication::translate("MainWindow", "Signals", Q_NULLPTR));
        SampleRateLabel->setText(QApplication::translate("MainWindow", "Sample Rate:", Q_NULLPTR));
        NumChannelsLabel->setText(QApplication::translate("MainWindow", "Number of Channels:", Q_NULLPTR));
        VolumeLabel_2->setText(QApplication::translate("MainWindow", "Signal Volume:", Q_NULLPTR));
        PRBLabel->setText(QApplication::translate("MainWindow", "Periods in Ring Buffer:", Q_NULLPTR));
        RBSLabel->setText(QApplication::translate("MainWindow", "Ring Buffer Size (ms):", Q_NULLPTR));
        DurationLabel->setText(QApplication::translate("MainWindow", "Measurement Duration (seconds):", Q_NULLPTR));
        Label1_6->setText(QApplication::translate("MainWindow", "Signal Detection Window:", Q_NULLPTR));
        SigDW->setSuffix(QString());
        Label1_7->setText(QApplication::translate("MainWindow", "Signal Detection Threshold:", Q_NULLPTR));
        Label1_8->setText(QApplication::translate("MainWindow", "Glitch Detection Window:", Q_NULLPTR));
        GliDW->setSuffix(QString());
        Label1_9->setText(QApplication::translate("MainWindow", "Glitch Detection Threshold:", Q_NULLPTR));
        Label1_10->setText(QApplication::translate("MainWindow", "Max Value per Frame Dump:", Q_NULLPTR));
        MVFD->setSuffix(QString());
        Label1_11->setText(QApplication::translate("MainWindow", "Dump Rounding:", Q_NULLPTR));
        DR->setSuffix(QString());
        Label1->setText(QApplication::translate("MainWindow", "Simple Moving Average Window:", Q_NULLPTR));
        SMA->setSuffix(QApplication::translate("MainWindow", " seconds", Q_NULLPTR));
        Label1_2->setText(QApplication::translate("MainWindow", "Strike Period:", Q_NULLPTR));
        StrikePeriod->setSuffix(QApplication::translate("MainWindow", " seconds", Q_NULLPTR));
        Label1_3->setText(QApplication::translate("MainWindow", "Strike Length:", Q_NULLPTR));
        Label1_4->setText(QApplication::translate("MainWindow", "Strike Detection Window:", Q_NULLPTR));
        StrDW->setSuffix(QString());
        Label1_5->setText(QApplication::translate("MainWindow", "Strike Detection Threshold:", Q_NULLPTR));
        Tabs->setTabText(Tabs->indexOf(Options), QApplication::translate("MainWindow", "Options", Q_NULLPTR));
        ErrorLabel->setText(QString());
        ErrorLabel_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
