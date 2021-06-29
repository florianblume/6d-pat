# This check is only there for when you build the project with your
# own Qt libraries
lessThan(QT_MINOR_VERSION, 14) {
    error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
    }
}

TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11 no_keywords

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/include/python3.8 \
               /usr/include/pybind11
LIBS += -lpython3.8

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv4

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
