TARGET = 6DPAT

TEMPLATE = app

QT += core gui widgets

DEPENDPATH += ../src
INCLUDEPATH += ../src

LIBS += -L$$OUT_PWD/../src -l6dpat

SOURCES += main.cpp

include(../defaults.pri)
