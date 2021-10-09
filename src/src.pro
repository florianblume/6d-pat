
# This check is only there for when you build the project with your
# own Qt libraries
lessThan(QT_MINOR_VERSION, 14) {
    error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
    }
}

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

TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender 3dinput
CONFIG += c++11 no_keywords

DEFINES += QT_DEPRECATED_WARNINGS PYBIND11_PYTHON_VERSION="3.8"

INCLUDEPATH += /usr/include/python3.8 \
               /usr/include/pybind11

LIBS += -lpython3.8

include(controller/controller.pri)
include(misc/misc.pri)
include(model/model.pri)
include(settings/settings.pri)
include(view/view.pri)
include(3dparty/QtAwesome/QtAwesome/QtAwesome.pri)

SOURCES += main.cpp \

RESOURCES += resources/shaders/shaders.qrc \
             resources/images/images.qrc \
             resources/fonts/fonts.qrc \
             resources/stylesheets/stylesheets.qrc
