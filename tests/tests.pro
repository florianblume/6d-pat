TEMPLATE = app
QT += core testlib
CONFIG += c++11 testcase no_testcase_installs no_keywords

include(../defaults.pri)
include(model/model.pri)
include(view/view.pri)
include(controller/controller.pri)
include(integration/integration.pri)

RESOURCES += \
    resources.qrc

DEPENDPATH += ../src
INCLUDEPATH += ../src

LIBS += -L$$OUT_PWD/../src -l6dpat
