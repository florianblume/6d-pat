QT       += core gui

CONFIG += c++11 no_keywords

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
    $$PWD/src/main/model/correspondence.hpp \
    $$PWD/src/main/misc/global.h \
    $$PWD/src/main/misc/preferences/preferences.hpp \
    $$PWD/src/main/misc/preferences/preferencesstore.hpp \
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
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.hpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/backgroundimagerenderable.hpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/correspondenceviewerglwidget.hpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/clickvisualizationoverlay.hpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/correspondencerenderable.hpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.hpp \
    $$PWD/src/main/view/correspondenceeditor/rendering/correspondenceeditorglwidget.hpp \
    $$PWD/src/main/view/correspondenceeditor/rendering/objectmodelrenderable.hpp \
    $$PWD/src/main/misc/generalhelper.h \
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.hpp \
    $$PWD/src/main/controller/neuralnetworkcontroller.hpp \
    $$PWD/src/main/controller/neuralnetworkthread.hpp \
    $$PWD/src/main/controller/correspondencecreator.hpp \
    $$PWD/src/main/view/settings/settingsnetworkpage.hpp

SOURCES += \
    $$PWD/src/main/view/mainwindow.cpp \
    $$PWD/src/main/controller/maincontroller.cpp \
    $$PWD/src/main/controller/correspondencecreator.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/model/loadandstorestrategy.cpp \
    $$PWD/src/main/model/cachingmodelmanager.cpp \
    $$PWD/src/main/model/modelmanager.cpp \
    $$PWD/src/main/model/jsonloadandstorestrategy.cpp \
    $$PWD/src/main/model/correspondence.cpp \
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
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.cpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/backgroundimagerenderable.cpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/correspondenceviewerglwidget.cpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/correspondencerenderable.cpp \
    $$PWD/src/main/view/correspondenceviewer/rendering/clickvisualizationoverlay.cpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.cpp \
    $$PWD/src/main/view/correspondenceeditor/rendering/correspondenceeditorglwidget.cpp \
    $$PWD/src/main/view/correspondenceeditor/rendering/objectmodelrenderable.cpp \
    $$PWD/src/main/misc/preferences/preferencesstore.cpp\
    $$PWD/src/main/misc/preferences/preferences.cpp \
    $$PWD/src/main/misc/generalhelper.cpp \
    $$PWD/src/main/view/misc/displayhelper.cpp \
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.cpp \
    $$PWD/src/main/controller/neuralnetworkcontroller.cpp \
    $$PWD/src/main/controller/neuralnetworkthread.cpp

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/settings/settingsdialog.ui \
    $$PWD/src/main/view/settings/settingsgeneralpage.ui \
    $$PWD/src/main/view/settings/settingsnetworkpage.ui \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.ui \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.ui

RESOURCES += \
    $$PWD/6dpatsources.qrc

INCLUDEPATH += $$PWD/src/main \
    $$PWD/src/main/model \
    $$PWD/src/main/view \
    $$PWD/src/main/controller \
    $$PWD/3dparty/QtAwesome \
    /usr/local/include/opencv \
    /usr/local/include/assimp \
    /home/floretti/anaconda3/envs/pose_estimation/include/python3.5m

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d \
        -L/usr/local/lib -lassimp

LIBS += -L/home/floretti/Qt5.11.0/5.11.0/gcc_64/lib -lQt5Core -lQt5Gui -lQt5Widgets \
        -L/home/floretti/anaconda3/envs/pose_estimation/lib -lpython3.5m
