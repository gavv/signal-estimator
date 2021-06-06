/********************************************************************************
** Form generated from reading UI file 'sigestnotfound.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGESTNOTFOUND_H
#define UI_SIGESTNOTFOUND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SigEstNotFound
{
public:
    QPushButton *pushButton;
    QLabel *label;

    void setupUi(QDialog *SigEstNotFound)
    {
        if (SigEstNotFound->objectName().isEmpty())
            SigEstNotFound->setObjectName(QStringLiteral("SigEstNotFound"));
        SigEstNotFound->resize(400, 165);
        pushButton = new QPushButton(SigEstNotFound);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(80, 110, 241, 41));
        label = new QLabel(SigEstNotFound);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 381, 91));
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        retranslateUi(SigEstNotFound);

        QMetaObject::connectSlotsByName(SigEstNotFound);
    } // setupUi

    void retranslateUi(QDialog *SigEstNotFound)
    {
        SigEstNotFound->setWindowTitle(QApplication::translate("SigEstNotFound", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("SigEstNotFound", "Close", Q_NULLPTR));
        label->setText(QApplication::translate("SigEstNotFound", "The Signal-Estimator executable was not found in the current directory ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SigEstNotFound: public Ui_SigEstNotFound {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGESTNOTFOUND_H
