#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T16:52:08
#
#-------------------------------------------------

QT       += core gui 3dcore 3drender 3dinput 3dextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./OtiatProgram.pro)
include(QtAwesome/QtAwesome.pri)

CONFIG += c++11

TARGET = Otiat
TEMPLATE = app

SOURCES += \
    src/main/main.cpp

DISTFILES += \
    Otiat.pri


