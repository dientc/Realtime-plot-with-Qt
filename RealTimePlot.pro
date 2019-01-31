#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T09:44:25
#
#-------------------------------------------------

QT       += core gui printsupport
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RealTimePlot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
