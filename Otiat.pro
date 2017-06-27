#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T16:52:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./Otiat.pri)

TARGET = Otiat
TEMPLATE = app

SOURCES += \
    src/main/main.cpp

LIBS += \
       -lboost_system\
       -lboost_filesystem\

FORMS    += \
    resources/mainwindow.ui

DISTFILES += \
    Otiat.pri
