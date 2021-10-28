TARGET = 6DPAT

TEMPLATE = app

QT += core gui widgets

LIBS += -L../src -l6dpat

SOURCES += main.cpp

include(../defaults.pri)
