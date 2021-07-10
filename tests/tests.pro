TEMPLATE = app
CONFIG += c++11 testcase
QT += testlib

include(model/model.pri)
include(view/view.pri)
include(controller/controller.pri)

SOURCES += main.cpp
