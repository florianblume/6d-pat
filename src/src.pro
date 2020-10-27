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

QT     += core gui widgets 3dcore 3dextras 3drender 3drender-private
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/local/include/opencv4 \
            += ../../../include

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_calib3d \
        -L../../../lib/ -lqt3dwidget

HEADERS  += \
    mainapplication.hpp \
    controller/maincontroller.hpp \
    model/cachingmodelmanager.hpp \
    model/image.hpp \
    model/loadandstorestrategy.hpp \
    model/modelmanager.hpp \
    model/objectmodel.hpp \
    model/jsonloadandstorestrategy.hpp \
    model/pose.hpp \
    view/misc/displayhelper.hpp \
    view/mainwindow.hpp \
    view/breadcrumb/breadcrumbview.hpp \
    view/gallery/gallery.hpp \
    view/gallery/galleryimagemodel.hpp \
    view/gallery/galleryobjectmodelmodel.hpp \
    view/gallery/iconexpandinglistview.hpp \
    view/navigationcontrols/navigationcontrols.hpp \
    view/rendering/backgroundimagerenderable.hpp \
    view/rendering/poserenderable.hpp \
    view/settings/settingsdialog.hpp \
    view/settings/settingsgeneralpage.hpp \
    view/settings/settingssegmentationcodespage.hpp \
    view/poseviewer/poseviewer.hpp \
    view/poseviewer/poseviewer3dwidget.hpp \
    view/poseeditor/poseeditor.hpp \
    view/poseeditor/poseeditor3dwidget.hpp \
    view/settings/settingsnetworkpage.hpp \
    view/neuralnetworkdialog/neuralnetworkdialog.hpp \
    view/gallery/resizeimagesrunnable.hpp \
    view/neuralnetworkprogressview/networkprogressview.hpp \
    settings/settings.hpp \
    settings/settingsstore.hpp \
    controller/poserecoverer.hpp \
    controller/neuralnetworkcontroller.hpp \
    controller/neuralnetworkrunnable.hpp \
    misc/generalhelper.hpp \
    misc/global.hpp \
    view/gallery/rendering/offscreenengine.h \
    view/gallery/rendering/texturerendertarget.h \
    view/rendering/objectmodelrenderable.hpp \
    view/rendering/objectmodelrenderablematerial.h \
    view/poseeditor/listviewposesmodel.h \
    view/rendering/clickvisualizationmaterial.hpp \
    view/rendering/clickvisualizationrenderable.hpp

SOURCES += \
    main.cpp \
    mainapplication.cpp \
    view/mainwindow.cpp \
    controller/maincontroller.cpp \
    model/image.cpp \
    model/objectmodel.cpp \
    model/loadandstorestrategy.cpp \
    model/cachingmodelmanager.cpp \
    model/modelmanager.cpp \
    model/jsonloadandstorestrategy.cpp \
    model/pose.cpp \
    view/breadcrumb/breadcrumbview.cpp \
    view/navigationcontrols/navigationcontrols.cpp \
    view/gallery/gallery.cpp \
    view/gallery/galleryimagemodel.cpp \
    view/gallery/galleryobjectmodelmodel.cpp \
    view/gallery/iconexpandinglistview.cpp \
    view/rendering/backgroundimagerenderable.cpp \
    view/rendering/poserenderable.cpp \
    view/settings/settingsdialog.cpp \
    view/settings/settingsgeneralpage.cpp \
    view/settings/settingsnetworkpage.cpp \
    view/settings/settingssegmentationcodespage.cpp \
    view/poseviewer/poseviewer.cpp \
    view/poseviewer/poseviewer3dwidget.cpp \
    view/poseeditor/poseeditor.cpp \
    view/poseeditor/poseeditor3dwidget.cpp \
    view/neuralnetworkdialog/neuralnetworkdialog.cpp \
    view/gallery/resizeimagesrunnable.cpp \
    view/neuralnetworkprogressview/networkprogressview.cpp \
    view/misc/displayhelper.cpp \
    controller/neuralnetworkcontroller.cpp \
    controller/neuralnetworkrunnable.cpp \
    controller/poserecoverer.cpp \
    settings/settings.cpp \
    settings/settingsstore.cpp \
    misc/generalhelper.cpp \
    view/gallery/rendering/offscreenengine.cpp \
    view/gallery/rendering/texturerendertarget.cpp \
    view/rendering/objectmodelrenderable.cpp \
    view/rendering/objectmodelrenderablematerial.cpp \
    view/poseeditor/listviewposesmodel.cpp \
    view/rendering/clickvisualizationmaterial.cpp \
    view/rendering/clickvisualizationrenderable.cpp

FORMS    += \
    view/mainwindow.ui \
    view/navigationcontrols/navigationcontrols.ui \
    view/gallery/gallery.ui \
    view/settings/settingsdialog.ui \
    view/settings/settingsgeneralpage.ui \
    view/settings/settingsnetworkpage.ui \
    view/settings/settingssegmentationcodespage.ui \
    view/poseviewer/poseviewer.ui \
    view/poseeditor/poseeditor.ui \
    view/neuralnetworkdialog/neuralnetworkdialog.ui \
    view/neuralnetworkprogressview/networkprogressview.ui

RESOURCES += src.qrc
