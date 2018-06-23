#include(./OtiatSources.pri)
include(./gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

TARGET = OtiatTests

SOURCES += \
    $$PWD/src/test/testmain.cpp

HEADERS +=     $$PWD/src/test/tst_modeltests.h

DISTFILES = \
    OtiatSources.pri
