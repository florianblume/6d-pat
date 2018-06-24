QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./3dparty/QtAwesome/QtAwesome.pri)

HEADERS  += \
    $$PWD/src/main/controller/maincontroller.hpp \
    $$PWD/src/main/controller/correspondencecreator.h \
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
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.hpp

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
    $$PWD/src/main/view/gallery/rendering/offscreenrenderer.cpp

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/settings/settingsdialog.ui \
    $$PWD/src/main/view/settings/settingsgeneralpage.ui \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.ui \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.ui

INCLUDEPATH += \
    $$PWD/src/main \
    $$PWD/src/main/model \
    $$PWD/src/main/view \
    $$PWD/src/main/controller \
    $$PWD/3dparty/QtAwesome \
    /usr/local/include/opencv

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d \
        -L/usr/local/lib -lassimp

INCLUDEPATH += /usr/local/include/assimp

RESOURCES += \
    $$PWD/6dpatsources.qrc
