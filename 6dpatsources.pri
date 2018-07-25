QT       += core gui concurrent

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./3dparty/QtAwesome/QtAwesome.pri)

HEADERS  += \
    $$PWD/src/main/controller/maincontroller.hpp \
    $$PWD/src/main/model/cachingmodelmanager.hpp \
    $$PWD/src/main/model/image.hpp \
    $$PWD/src/main/model/loadandstorestrategy.hpp \
    $$PWD/src/main/model/modelmanager.hpp \
    $$PWD/src/main/model/objectmodel.hpp \
    $$PWD/src/main/model/jsonloadandstorestrategy.hpp \
    $$PWD/src/main/model/pose.hpp \
    $$PWD/src/main/misc/global.h \
    $$PWD/src/main/view/misc/displayhelper.h \
    $$PWD/src/main/view/mainwindow.hpp \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.hpp \
    $$PWD/src/main/view/gallery/gallery.hpp \
    $$PWD/src/main/view/gallery/galleryimagemodel.hpp \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.hpp \
    $$PWD/src/main/view/gallery/iconexpandinglistview.hpp \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.hpp \
    $$PWD/src/main/view/settings/settingsdialog.hpp \
    $$PWD/src/main/view/settings/settingsgeneralpage.hpp \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.hpp \
    $$PWD/src/main/view/poseviewer/poseviewer.hpp \
    $$PWD/src/main/view/poseviewer/rendering/backgroundimagerenderable.hpp \
    $$PWD/src/main/view/poseviewer/rendering/poseviewerglwidget.hpp \
    $$PWD/src/main/view/poseviewer/rendering/clickvisualizationoverlay.hpp \
    $$PWD/src/main/view/poseviewer/rendering/poserenderable.hpp \
    $$PWD/src/main/view/poseeditor/poseeditor.hpp \
    $$PWD/src/main/view/poseeditor/rendering/poseeditorglwidget.hpp \
    $$PWD/src/main/view/poseeditor/rendering/objectmodelrenderable.hpp \
    $$PWD/src/main/misc/generalhelper.h \
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.hpp \
    $$PWD/src/main/controller/neuralnetworkcontroller.hpp \
    $$PWD/src/main/controller/posecreator.hpp \
    $$PWD/src/main/view/settings/settingsnetworkpage.hpp \
    $$PWD/src/main/controller/neuralnetworkrunnable.hpp \
    $$PWD/src/main/view/neuralnetworkdialog/neuralnetworkdialog.hpp \
    $$PWD/src/main/view/gallery/resizeimagesrunnable.h \
    $$PWD/src/main/view/neuralnetworkprogressview/networkprogressview.hpp \
    $$PWD/src/main/settings/settings.hpp \
    $$PWD/src/main/settings/settingsstore.hpp

SOURCES += \
    $$PWD/src/main/view/mainwindow.cpp \
    $$PWD/src/main/controller/maincontroller.cpp \
    $$PWD/src/main/controller/posecreator.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/model/loadandstorestrategy.cpp \
    $$PWD/src/main/model/cachingmodelmanager.cpp \
    $$PWD/src/main/model/modelmanager.cpp \
    $$PWD/src/main/model/jsonloadandstorestrategy.cpp \
    $$PWD/src/main/model/pose.cpp \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.cpp \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.cpp \
    $$PWD/src/main/view/gallery/gallery.cpp \
    $$PWD/src/main/view/gallery/galleryimagemodel.cpp \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.cpp \
    $$PWD/src/main/view/gallery/iconexpandinglistview.cpp \
    $$PWD/src/main/view/settings/settingsdialog.cpp \
    $$PWD/src/main/view/settings/settingsgeneralpage.cpp \
    $$PWD/src/main/view/settings/settingsnetworkpage.cpp \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.cpp \
    $$PWD/src/main/view/poseviewer/poseviewer.cpp \
    $$PWD/src/main/view/poseviewer/rendering/backgroundimagerenderable.cpp \
    $$PWD/src/main/view/poseviewer/rendering/poseviewerglwidget.cpp \
    $$PWD/src/main/view/poseviewer/rendering/poserenderable.cpp \
    $$PWD/src/main/view/poseviewer/rendering/clickvisualizationoverlay.cpp \
    $$PWD/src/main/view/poseeditor/poseeditor.cpp \
    $$PWD/src/main/view/poseeditor/rendering/poseeditorglwidget.cpp \
    $$PWD/src/main/view/poseeditor/rendering/objectmodelrenderable.cpp \
    $$PWD/src/main/misc/generalhelper.cpp \
    $$PWD/src/main/view/misc/displayhelper.cpp \
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.cpp \
    $$PWD/src/main/controller/neuralnetworkcontroller.cpp \
    $$PWD/src/main/controller/neuralnetworkrunnable.cpp \
    $$PWD/src/main/view/neuralnetworkdialog/neuralnetworkdialog.cpp \
    $$PWD/src/main/view/gallery/resizeimagesrunnable.cpp \
    $$PWD/src/main/view/neuralnetworkprogressview/networkprogressview.cpp \
    $$PWD/src/main/settings/settings.cpp \
    $$PWD/src/main/settings/settingsstore.cpp

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/settings/settingsdialog.ui \
    $$PWD/src/main/view/settings/settingsgeneralpage.ui \
    $$PWD/src/main/view/settings/settingsnetworkpage.ui \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/src/main/view/poseviewer/poseviewer.ui \
    $$PWD/src/main/view/poseeditor/poseeditor.ui \
    $$PWD/src/main/view/neuralnetworkdialog/neuralnetworkdialog.ui \
    $$PWD/src/main/view/neuralnetworkprogressview/networkprogressview.ui

RESOURCES += \
    $$PWD/6dpatsources.qrc

INCLUDEPATH += $$PWD/src/main \
    $$PWD/3dparty/QtAwesome \
    /usr/local/include/opencv \
    /usr/local/include/assimp

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d \
        -L/usr/local/lib -lassimp
