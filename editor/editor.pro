#-------------------------------------------------
#
# Project created by QtCreator 2015-08-12T14:57:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = ampcontroleditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lcdconverter.cpp

HEADERS  += mainwindow.h \
    lcdconverter.h

FORMS    += mainwindow.ui
