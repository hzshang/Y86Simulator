#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T18:57:54
#
#-------------------------------------------------

QT += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Y86-Simultor-Net
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        maindialog.cpp \
    clock.cpp \
    y86Control/fetch.cpp \
    y86Control/decode.cpp \
    y86Control/execute.cpp \
    y86Control/memory.cpp \
    y86Control/writeback.cpp \
    y86.cpp \
    mainwindow.cpp

HEADERS  += maindialog.h \
    clock.h \
    y86Control/fetch.h \
    y86Control/decode.h \
    y86Control/execute.h \
    y86Control/memory.h \
    y86Control/writeback.h \
    y86.h \
    const.h \
    mainwindow.h

FORMS    += maindialog.ui \
    mainwindow.ui
