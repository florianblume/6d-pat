TEMPLATE = app
QT += core testlib
CONFIG += c++11 testcase no_testcase_installs no_keywords

include(../defaults.pri)
include(model/model.pri)
include(view/view.pri)
include(controller/controller.pri)
include(integration/integration.pri)

LIBS += -L../src -l6dpat

RESOURCES += \
    resources.qrc



unix: QT_CONFIG -= no-pkg-config
unix: CONFIG += link_pkgconfig

packagesExist(opencv) {
    unix: PKGCONFIG += opencv
} else {
    packagesExist(opencv4) {
        unix: PKGCONFIG += opencv4
    } else {
        error(OpenCV not found!)
    }
}

DEFINES += QT_DEPRECATED_WARNINGS PYBIND11_PYTHON_VERSION="3.8"

INCLUDEPATH += /usr/include/python3.8 \
               /usr/include/pybind11
