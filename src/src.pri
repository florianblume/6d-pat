include(../defaults.pri)

CONFIG += c++11
QT     += core gui

TEMPLATE = lib
TARGET = 6dpat_lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./3rdparty/QtAwesome/QtAwesome.pri)

INCLUDEPATH += $$PWD/main \
               $$PWD/3rdparty/QtAwesome \
               /usr/local/include/opencv4 \
               /usr/include/assimp

LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_calib3d \
        -L/usr/lib/ -lassimp

HEADERS  += \
    $$PWD/main/controller/maincontroller.hpp \
    $$PWD/main/model/cachingmodelmanager.hpp \
    $$PWD/main/model/image.hpp \
    $$PWD/main/model/loadandstorestrategy.hpp \
    $$PWD/main/model/modelmanager.hpp \
    $$PWD/main/model/objectmodel.hpp \
    $$PWD/main/model/jsonloadandstorestrategy.hpp \
    $$PWD/main/model/pose.hpp \
    $$PWD/main/misc/global.h \
    $$PWD/main/view/misc/displayhelper.h \
    $$PWD/main/view/mainwindow.hpp \
    $$PWD/main/view/breadcrumb/breadcrumbview.hpp \
    $$PWD/main/view/gallery/gallery.hpp \
    $$PWD/main/view/gallery/galleryimagemodel.hpp \
    $$PWD/main/view/gallery/galleryobjectmodelmodel.hpp \
    $$PWD/main/view/gallery/iconexpandinglistview.hpp \
    $$PWD/main/view/navigationcontrols/navigationcontrols.hpp \
    $$PWD/main/view/rendering/backgroundimagerenderable.hpp \
    $$PWD/main/view/rendering/objectmodelrenderable.hpp \
    $$PWD/main/view/rendering/poserenderable.hpp \
    $$PWD/main/view/settings/settingsdialog.hpp \
    $$PWD/main/view/settings/settingsgeneralpage.hpp \
    $$PWD/main/view/settings/settingssegmentationcodespage.hpp \
    $$PWD/main/view/poseviewer/poseviewer.hpp \
    $$PWD/main/view/poseviewer/rendering/poseviewerglwidget.hpp \
    $$PWD/main/view/poseviewer/rendering/clickvisualizationoverlay.hpp \
    $$PWD/main/view/poseviewer/rendering/poserenderable.hpp \
    $$PWD/main/view/poseeditor/poseeditor.hpp \
    $$PWD/main/view/poseeditor/rendering/poseeditorglwidget.hpp \
    $$PWD/main/view/poseeditor/rendering/objectmodelrenderable.hpp \
    $$PWD/main/misc/generalhelper.h \
    $$PWD/main/view/gallery/rendering/offscreenrenderer.hpp \
    $$PWD/main/controller/neuralnetworkcontroller.hpp \
    $$PWD/main/view/settings/settingsnetworkpage.hpp \
    $$PWD/main/controller/neuralnetworkrunnable.hpp \
    $$PWD/main/view/neuralnetworkdialog/neuralnetworkdialog.hpp \
    $$PWD/main/view/gallery/resizeimagesrunnable.h \
    $$PWD/main/view/neuralnetworkprogressview/networkprogressview.hpp \
    $$PWD/main/settings/settings.hpp \
    $$PWD/main/settings/settingsstore.hpp \
    $$PWD/main/controller/poserecoverer.hpp

SOURCES += \
    $$PWD/main/view/mainwindow.cpp \
    $$PWD/main/controller/maincontroller.cpp \
    $$PWD/main/model/image.cpp \
    $$PWD/main/model/objectmodel.cpp \
    $$PWD/main/model/loadandstorestrategy.cpp \
    $$PWD/main/model/cachingmodelmanager.cpp \
    $$PWD/main/model/modelmanager.cpp \
    $$PWD/main/model/jsonloadandstorestrategy.cpp \
    $$PWD/main/model/pose.cpp \
    $$PWD/main/view/breadcrumb/breadcrumbview.cpp \
    $$PWD/main/view/navigationcontrols/navigationcontrols.cpp \
    $$PWD/main/view/gallery/gallery.cpp \
    $$PWD/main/view/gallery/galleryimagemodel.cpp \
    $$PWD/main/view/gallery/galleryobjectmodelmodel.cpp \
    $$PWD/main/view/gallery/iconexpandinglistview.cpp \
    $$PWD/main/view/rendering/backgroundimagerenderable.cpp \
    $$PWD/main/view/rendering/objectmodelrenderable.cpp \
    $$PWD/main/view/rendering/poserenderable.cpp \
    $$PWD/main/view/settings/settingsdialog.cpp \
    $$PWD/main/view/settings/settingsgeneralpage.cpp \
    $$PWD/main/view/settings/settingsnetworkpage.cpp \
    $$PWD/main/view/settings/settingssegmentationcodespage.cpp \
    $$PWD/main/view/poseviewer/poseviewer.cpp \
    $$PWD/main/view/poseviewer/rendering/poseviewerglwidget.cpp \
    $$PWD/main/view/poseviewer/rendering/clickvisualizationoverlay.cpp \
    $$PWD/main/view/poseeditor/poseeditor.cpp \
    $$PWD/main/view/poseeditor/rendering/poseeditorglwidget.cpp \
    $$PWD/main/misc/generalhelper.cpp \
    $$PWD/main/view/misc/displayhelper.cpp \
    $$PWD/main/view/gallery/rendering/offscreenrenderer.cpp \
    $$PWD/main/controller/neuralnetworkcontroller.cpp \
    $$PWD/main/controller/neuralnetworkrunnable.cpp \
    $$PWD/main/view/neuralnetworkdialog/neuralnetworkdialog.cpp \
    $$PWD/main/view/gallery/resizeimagesrunnable.cpp \
    $$PWD/main/view/neuralnetworkprogressview/networkprogressview.cpp \
    $$PWD/main/settings/settings.cpp \
    $$PWD/main/settings/settingsstore.cpp \
    $$PWD/main/controller/poserecoverer.cpp

FORMS    += \
    $$PWD/main/view/mainwindow.ui \
    $$PWD/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/main/view/gallery/gallery.ui \
    $$PWD/main/view/settings/settingsdialog.ui \
    $$PWD/main/view/settings/settingsgeneralpage.ui \
    $$PWD/main/view/settings/settingsnetworkpage.ui \
    $$PWD/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/main/view/poseviewer/poseviewer.ui \
    $$PWD/main/view/poseeditor/poseeditor.ui \
    $$PWD/main/view/neuralnetworkdialog/neuralnetworkdialog.ui \
    $$PWD/main/view/neuralnetworkprogressview/networkprogressview.ui

RESOURCES += src.qrc
