#-------------------------------------------------
#
# Project created by QtCreator 2021-05-21T13:02:22
#
#-------------------------------------------------

QT       += core gui
include(/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qwt.prf)
QMAKE_CXXFLAGS += -std=c++17
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Signal-Estimator-GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/Signal-Estimator-GUI/ipc.cpp \
    src/Signal-Estimator-GUI/main.cpp \
    src/Signal-Estimator-GUI/mainwindow.cpp \
    src/Signal-Estimator-GUI/points.cpp \
    src/Signal-Estimator-GUI/sigestnotfound.cpp

HEADERS += \
    3rdparty/Ring-Buffer-master/ringbuffer.hpp \
    src/Signal-Estimator-GUI/ipc.h \
    src/Signal-Estimator-GUI/mainwindow.h \
    src/Signal-Estimator-GUI/points.h \
    src/Signal-Estimator-GUI/sigestnotfound.h



FORMS += \
    src/Signal-Estimator-GUI/mainwindow.ui \
    src/Signal-Estimator-GUI/sigestnotfound.ui

SUBDIRS += \


DISTFILES += \
    "src/Signal-Estimator-GUI/GUI build/signal-estimator" \
    "src/Signal-Estimator-GUI/GUI build/Signal-Estimator-GUI"

