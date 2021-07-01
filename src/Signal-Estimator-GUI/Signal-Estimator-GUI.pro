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
        mainwindow.cpp \
    main.cpp \
    sigestnotfound.cpp \
    ipc.cpp \
    points.cpp

HEADERS += \
        mainwindow.h \
    sigestnotfound.h \
    ipc.h \
    ../../3rdparty/Ring-Buffer-master/ringbuffer.hpp \
    points.h

FORMS += \
        mainwindow.ui \
    sigestnotfound.ui
