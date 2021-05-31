/********************************************************************************
** Form generated from reading UI file 'extra_options.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRA_OPTIONS_H
#define UI_EXTRA_OPTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_extra_options
{
public:
    QLabel *Label1;
    QSpinBox *SMA;
    QLabel *Label1_2;
    QSpinBox *StrikePeriod;
    QDoubleSpinBox *StrikeLength;
    QLabel *Label1_3;
    QSpinBox *StrDW;
    QLabel *Label1_4;
    QDoubleSpinBox *StrDT;
    QLabel *Label1_5;
    QLabel *Label1_6;
    QSpinBox *SigDW;
    QDoubleSpinBox *SigDT;
    QLabel *Label1_7;
    QSpinBox *GliDW;
    QLabel *Label1_8;
    QDoubleSpinBox *GliDT;
    QLabel *Label1_9;
    QLabel *Label1_10;
    QSpinBox *MVFD;
    QLabel *Label1_11;
    QSpinBox *DR;
    QPushButton *OkButton;

    void setupUi(QDialog *extra_options)
    {
        if (extra_options->objectName().isEmpty())
            extra_options->setObjectName(QStringLiteral("extra_options"));
        extra_options->resize(700, 370);
        Label1 = new QLabel(extra_options);
        Label1->setObjectName(QStringLiteral("Label1"));
        Label1->setGeometry(QRect(10, 10, 231, 31));
        SMA = new QSpinBox(extra_options);
        SMA->setObjectName(QStringLiteral("SMA"));
        SMA->setGeometry(QRect(240, 10, 121, 31));
        SMA->setMaximum(2147483647);
        SMA->setValue(5);
        Label1_2 = new QLabel(extra_options);
        Label1_2->setObjectName(QStringLiteral("Label1_2"));
        Label1_2->setGeometry(QRect(10, 50, 111, 31));
        StrikePeriod = new QSpinBox(extra_options);
        StrikePeriod->setObjectName(QStringLiteral("StrikePeriod"));
        StrikePeriod->setGeometry(QRect(120, 50, 171, 31));
        StrikePeriod->setMaximum(2147483647);
        StrikePeriod->setValue(1);
        StrikeLength = new QDoubleSpinBox(extra_options);
        StrikeLength->setObjectName(QStringLiteral("StrikeLength"));
        StrikeLength->setGeometry(QRect(120, 90, 171, 31));
        StrikeLength->setDecimals(6);
        StrikeLength->setMaximum(1);
        StrikeLength->setSingleStep(0.01);
        StrikeLength->setValue(0.01);
        Label1_3 = new QLabel(extra_options);
        Label1_3->setObjectName(QStringLiteral("Label1_3"));
        Label1_3->setGeometry(QRect(10, 90, 111, 31));
        StrDW = new QSpinBox(extra_options);
        StrDW->setObjectName(QStringLiteral("StrDW"));
        StrDW->setGeometry(QRect(190, 130, 171, 31));
        StrDW->setMinimum(0);
        StrDW->setMaximum(2147483647);
        StrDW->setValue(96);
        Label1_4 = new QLabel(extra_options);
        Label1_4->setObjectName(QStringLiteral("Label1_4"));
        Label1_4->setGeometry(QRect(10, 130, 181, 31));
        StrDT = new QDoubleSpinBox(extra_options);
        StrDT->setObjectName(QStringLiteral("StrDT"));
        StrDT->setGeometry(QRect(200, 170, 161, 31));
        StrDT->setDecimals(6);
        StrDT->setMaximum(1);
        StrDT->setSingleStep(0.01);
        StrDT->setValue(0.4);
        Label1_5 = new QLabel(extra_options);
        Label1_5->setObjectName(QStringLiteral("Label1_5"));
        Label1_5->setGeometry(QRect(10, 170, 191, 31));
        Label1_6 = new QLabel(extra_options);
        Label1_6->setObjectName(QStringLiteral("Label1_6"));
        Label1_6->setGeometry(QRect(380, 10, 181, 31));
        SigDW = new QSpinBox(extra_options);
        SigDW->setObjectName(QStringLiteral("SigDW"));
        SigDW->setGeometry(QRect(560, 10, 131, 31));
        SigDW->setMaximum(2147483647);
        SigDW->setValue(48);
        SigDT = new QDoubleSpinBox(extra_options);
        SigDT->setObjectName(QStringLiteral("SigDT"));
        SigDT->setGeometry(QRect(570, 50, 121, 31));
        SigDT->setDecimals(6);
        SigDT->setMaximum(1);
        SigDT->setSingleStep(0.01);
        SigDT->setValue(0.15);
        Label1_7 = new QLabel(extra_options);
        Label1_7->setObjectName(QStringLiteral("Label1_7"));
        Label1_7->setGeometry(QRect(380, 50, 191, 31));
        GliDW = new QSpinBox(extra_options);
        GliDW->setObjectName(QStringLiteral("GliDW"));
        GliDW->setGeometry(QRect(560, 90, 131, 31));
        GliDW->setMaximum(2147483647);
        GliDW->setValue(32);
        Label1_8 = new QLabel(extra_options);
        Label1_8->setObjectName(QStringLiteral("Label1_8"));
        Label1_8->setGeometry(QRect(380, 90, 181, 31));
        GliDT = new QDoubleSpinBox(extra_options);
        GliDT->setObjectName(QStringLiteral("GliDT"));
        GliDT->setGeometry(QRect(570, 130, 121, 31));
        GliDT->setDecimals(6);
        GliDT->setMaximum(1);
        GliDT->setSingleStep(0.01);
        GliDT->setValue(0.05);
        Label1_9 = new QLabel(extra_options);
        Label1_9->setObjectName(QStringLiteral("Label1_9"));
        Label1_9->setGeometry(QRect(380, 130, 191, 31));
        Label1_10 = new QLabel(extra_options);
        Label1_10->setObjectName(QStringLiteral("Label1_10"));
        Label1_10->setGeometry(QRect(380, 170, 191, 31));
        MVFD = new QSpinBox(extra_options);
        MVFD->setObjectName(QStringLiteral("MVFD"));
        MVFD->setGeometry(QRect(580, 170, 111, 31));
        MVFD->setMaximum(2147483647);
        MVFD->setValue(64);
        Label1_11 = new QLabel(extra_options);
        Label1_11->setObjectName(QStringLiteral("Label1_11"));
        Label1_11->setGeometry(QRect(380, 210, 121, 31));
        DR = new QSpinBox(extra_options);
        DR->setObjectName(QStringLiteral("DR"));
        DR->setGeometry(QRect(500, 210, 191, 31));
        DR->setMaximum(2147483647);
        DR->setValue(10);
        OkButton = new QPushButton(extra_options);
        OkButton->setObjectName(QStringLiteral("OkButton"));
        OkButton->setGeometry(QRect(500, 320, 181, 31));

        retranslateUi(extra_options);

        QMetaObject::connectSlotsByName(extra_options);
    } // setupUi

    void retranslateUi(QDialog *extra_options)
    {
        extra_options->setWindowTitle(QApplication::translate("extra_options", "Dialog", Q_NULLPTR));
        Label1->setText(QApplication::translate("extra_options", "Simple Moving Average Window:", Q_NULLPTR));
        SMA->setSuffix(QApplication::translate("extra_options", " seconds", Q_NULLPTR));
        Label1_2->setText(QApplication::translate("extra_options", "Strike Period:", Q_NULLPTR));
        StrikePeriod->setSuffix(QApplication::translate("extra_options", " seconds", Q_NULLPTR));
        Label1_3->setText(QApplication::translate("extra_options", "Strike Length:", Q_NULLPTR));
        StrDW->setSuffix(QString());
        Label1_4->setText(QApplication::translate("extra_options", "Strike Detection Window:", Q_NULLPTR));
        Label1_5->setText(QApplication::translate("extra_options", "Strike Detection Threshold:", Q_NULLPTR));
        Label1_6->setText(QApplication::translate("extra_options", "Signal Detection Window:", Q_NULLPTR));
        SigDW->setSuffix(QString());
        Label1_7->setText(QApplication::translate("extra_options", "Signal Detection Threshold:", Q_NULLPTR));
        GliDW->setSuffix(QString());
        Label1_8->setText(QApplication::translate("extra_options", "Glitch Detection Window:", Q_NULLPTR));
        Label1_9->setText(QApplication::translate("extra_options", "Glitch Detection Threshold:", Q_NULLPTR));
        Label1_10->setText(QApplication::translate("extra_options", "Max Value per Frame Dump:", Q_NULLPTR));
        MVFD->setSuffix(QString());
        Label1_11->setText(QApplication::translate("extra_options", "Dump Rounding:", Q_NULLPTR));
        DR->setSuffix(QString());
        OkButton->setText(QApplication::translate("extra_options", "Ok", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class extra_options: public Ui_extra_options {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRA_OPTIONS_H
