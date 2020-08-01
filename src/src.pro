lessThan(QT_MAJOR_VERSION, 5) {
    error(Needs at least Qt > 5.9 to run - found $$QT_VERSION.)
} else {
    lessThan(QT_MINOR_VERSION, 9) {
        error(Needs at least Qt > 5.9 to run - found $$QT_VERSION.)
    }
}

# If you have installed Assimp to a different location, remove the check or alter the location
!exists(/usr/lib/x86_64-linux-gnu/libassimp.so) {
    error(Assimp not found. Please install libassimp-dev.)
}

# If you have installed OpenCV to a different location, remove the check or alter the location
!exists(/usr/local/lib/libopencv_core.so){
    error(OpenCV not found. Please install it.)
}

include(../defaults.pri)
include(./3rdparty/QtAwesome/QtAwesome.pri)

TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/local/include/opencv4

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_calib3d

HEADERS  += \
    $$PWD/mainapplication.hpp \
    $$PWD/controller/maincontroller.hpp \
    $$PWD/model/cachingmodelmanager.hpp \
    $$PWD/model/image.hpp \
    $$PWD/model/loadandstorestrategy.hpp \
    $$PWD/model/modelmanager.hpp \
    $$PWD/model/objectmodel.hpp \
    $$PWD/model/jsonloadandstorestrategy.hpp \
    $$PWD/model/pose.hpp \
    $$PWD/view/misc/displayhelper.hpp \
    $$PWD/view/mainwindow.hpp \
    $$PWD/view/breadcrumb/breadcrumbview.hpp \
    $$PWD/view/gallery/gallery.hpp \
    $$PWD/view/gallery/galleryimagemodel.hpp \
    $$PWD/view/gallery/galleryobjectmodelmodel.hpp \
    $$PWD/view/gallery/iconexpandinglistview.hpp \
    $$PWD/view/navigationcontrols/navigationcontrols.hpp \
    $$PWD/view/rendering/backgroundimagerenderable.hpp \
    $$PWD/view/rendering/objectmodelrenderable.hpp \
    $$PWD/view/rendering/poserenderable.hpp \
    $$PWD/view/settings/settingsdialog.hpp \
    $$PWD/view/settings/settingsgeneralpage.hpp \
    $$PWD/view/settings/settingssegmentationcodespage.hpp \
    $$PWD/view/poseviewer/poseviewer.hpp \
    $$PWD/view/poseviewer/rendering/poseviewerglwidget.hpp \
    $$PWD/view/poseviewer/rendering/clickvisualizationoverlay.hpp \
    $$PWD/view/poseeditor/poseeditor.hpp \
    $$PWD/view/poseeditor/poseeditor3dwindow.hpp \
    $$PWD/view/settings/settingsnetworkpage.hpp \
    $$PWD/view/neuralnetworkdialog/neuralnetworkdialog.hpp \
    $$PWD/view/gallery/resizeimagesrunnable.hpp \
    $$PWD/view/neuralnetworkprogressview/networkprogressview.hpp \
    $$PWD/settings/settings.hpp \
    $$PWD/settings/settingsstore.hpp \
    $$PWD/controller/poserecoverer.hpp \
    $$PWD/controller/neuralnetworkcontroller.hpp \
    $$PWD/controller/neuralnetworkrunnable.hpp \
    $$PWD/misc/generalhelper.hpp \
    $$PWD/misc/global.hpp \
    $$PWD/view/gallery/rendering/offscreenengine.h \
    $$PWD/view/gallery/rendering/texturerendertarget.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainapplication.cpp \
    $$PWD/view/mainwindow.cpp \
    $$PWD/controller/maincontroller.cpp \
    $$PWD/model/image.cpp \
    $$PWD/model/objectmodel.cpp \
    $$PWD/model/loadandstorestrategy.cpp \
    $$PWD/model/cachingmodelmanager.cpp \
    $$PWD/model/modelmanager.cpp \
    $$PWD/model/jsonloadandstorestrategy.cpp \
    $$PWD/model/pose.cpp \
    $$PWD/view/breadcrumb/breadcrumbview.cpp \
    $$PWD/view/navigationcontrols/navigationcontrols.cpp \
    $$PWD/view/gallery/gallery.cpp \
    $$PWD/view/gallery/galleryimagemodel.cpp \
    $$PWD/view/gallery/galleryobjectmodelmodel.cpp \
    $$PWD/view/gallery/iconexpandinglistview.cpp \
    $$PWD/view/rendering/backgroundimagerenderable.cpp \
    $$PWD/view/rendering/objectmodelrenderable.cpp \
    $$PWD/view/rendering/poserenderable.cpp \
    $$PWD/view/settings/settingsdialog.cpp \
    $$PWD/view/settings/settingsgeneralpage.cpp \
    $$PWD/view/settings/settingsnetworkpage.cpp \
    $$PWD/view/settings/settingssegmentationcodespage.cpp \
    $$PWD/view/poseviewer/poseviewer.cpp \
    $$PWD/view/poseviewer/rendering/poseviewerglwidget.cpp \
    $$PWD/view/poseviewer/rendering/clickvisualizationoverlay.cpp \
    $$PWD/view/poseeditor/poseeditor.cpp \
    $$PWD/view/poseeditor/poseeditor3dwindow.cpp \
    $$PWD/view/neuralnetworkdialog/neuralnetworkdialog.cpp \
    $$PWD/view/gallery/resizeimagesrunnable.cpp \
    $$PWD/view/neuralnetworkprogressview/networkprogressview.cpp \
    $$PWD/view/misc/displayhelper.cpp \
    $$PWD/controller/neuralnetworkcontroller.cpp \
    $$PWD/controller/neuralnetworkrunnable.cpp \
    $$PWD/controller/poserecoverer.cpp \
    $$PWD/settings/settings.cpp \
    $$PWD/settings/settingsstore.cpp \
    $$PWD/misc/generalhelper.cpp \
    $$PWD/view/gallery/rendering/offscreenengine.cpp \
    $$PWD/view/gallery/rendering/texturerendertarget.cpp

FORMS    += \
    $$PWD/view/mainwindow.ui \
    $$PWD/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/view/gallery/gallery.ui \
    $$PWD/view/settings/settingsdialog.ui \
    $$PWD/view/settings/settingsgeneralpage.ui \
    $$PWD/view/settings/settingsnetworkpage.ui \
    $$PWD/view/settings/settingssegmentationcodespage.ui \
    $$PWD/view/poseviewer/poseviewer.ui \
    $$PWD/view/poseeditor/poseeditor.ui \
    $$PWD/view/neuralnetworkdialog/neuralnetworkdialog.ui \
    $$PWD/view/neuralnetworkprogressview/networkprogressview.ui

RESOURCES += src.qrc