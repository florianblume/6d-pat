TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

# This check is only there for when you build the project with your
# own Qt libraries
lessThan(QT_MINOR_VERSION, 14) {
    error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        error(Needs at least Qt >= 5.14 to run - found $$[QT_VERSION].)
    }
}

INCLUDEPATH += ../../../include/qt3dwidget \
            += /usr/local/include/opencv4

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_flann -lopencv_features2d -lopencv_calib3d \
        -L../../../lib/qt3dwidget -lqt3dwidget

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
