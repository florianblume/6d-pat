TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

# This check is only there for when you build the project with your
# own Qt libraries
lessThan(QT_MINOR_VERSION, 14) {
    error(Needs at least Qt >= 5.14 to run - found $$QT_VERSION.)
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        error(Needs at least Qt >= 5.14 to run - found $$QT_VERSION.)
    }
}

INCLUDEPATH += ../../../include/qt3dwidget \
            += /usr/include/opencv4/opencv2

LIBS += -L../../../lib/qt/lib -lQt5Core -lQt5Gui -lQt53DCore -lQt5Widgets -lQt53DInput -lQt53DRender -lQt53DExtras -lQt53DLogic \
        -L/usr/local/lib -lopencv_core -lopencv_calib3d \
        -L../../../lib/qt3dwidget/libqt3dwidget.a

LIBS = -lm $$LIBS

QMAKE_RPATHDIR += \$\$ORIGIN
QMAKE_RPATHDIR += \$\$ORIGIN/lib
QMAKE_RPATHDIR += \$\$ORIGIN/lib/qt
QMAKE_RPATHDIR += \$\$ORIGIN/lib/opencv4
RPATH = $$join( QMAKE_RPATHDIR, ":" )

QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${RPATH}\'
QMAKE_RPATHDIR =

#QMAKE_RPATHDIR += lib/qt
#QMAKE_RPATHDIR += lib/opencv4
#QMAKE_RPATHDIR += lib/qt3dwidget

#QMAKE_LFLAGS += "-Wl,-rpath,\'\$$PWD/lib/qt\'"
#QMAKE_LFLAGS += "-Wl,-rpath,\'\$$PWD/lib/opencv4\'"
#QMAKE_LFLAGS += "-Wl,-rpath,\'\$$PWD/lib/qt3dwidget\'"

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

