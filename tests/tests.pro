TEMPLATE = app
QT += core testlib
CONFIG += c++11 testcase no_testcase_installs

include(../defaults.pri)
include(model/model.pri)
include(view/view.pri)
include(controller/controller.pri)
include(integration/integration.pri)

LIBS += -L../src -l6dpat
