TEMPLATE = app
QT += core testlib
CONFIG += c++11 testcase no_testcase_installs no_keywords

include(../defaults.pri)
include(model/model.pri)
include(view/view.pri)
include(controller/controller.pri)
include(integration/integration.pri)

# We need to include Python again because we subclass
# PythonLoadAndStoreStrategy to access its private members
INCLUDEPATH += /usr/include/python3.8 \
               /usr/include/pybind11

LIBS += -lpython3.8

RESOURCES += \
    resources.qrc

DEPENDPATH += ../src
INCLUDEPATH += ../src

LIBS += -L$$OUT_PWD/../src -l6dpat
