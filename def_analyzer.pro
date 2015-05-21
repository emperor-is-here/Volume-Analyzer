#-------------------------------------------------
#
# Project created by QtCreator 2015-05-21T17:50:20
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = def_analyzer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    volumeanalyzer.cpp

HEADERS  += mainwindow.h \
    volumeanalyzer.h

FORMS    += mainwindow.ui
