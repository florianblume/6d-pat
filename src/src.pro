TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

lessThan(QT_MAJOR_VERSION, 5) {
    error(Needs at least Qt > 5.9 to run - found $$QT_VERSION.)
} else {
    lessThan(QT_MINOR_VERSION, 9) {
        error(Needs at least Qt > 5.9 to run - found $$QT_VERSION.)
    }
}

# If you have installed OpenCV to a different location, remove the check or alter the location
!exists(/usr/local/lib/libopencv_core.so){
    error(OpenCV not found. Please install it.)
}

INCLUDEPATH += /usr/local/include/opencv4 \
            += ../../../include

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_calib3d \
        -L../../../lib/ -lqt3dwidget

include(controller/controller.pri)
include(misc/misc.pri)
include(model/model.pri)
include(settings/settings.pri)
include(view/view.pri)

SOURCES += main.cpp \

RESOURCES += resources/shaders/shaders.qrc \
             resources/images/images.qrc \
             resources/fonts/fonts.qrc

