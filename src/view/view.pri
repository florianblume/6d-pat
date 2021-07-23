INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/poseviewer/renderframegraph.hpp \
    view/poseviewer/mousecoordinatesmodificationeventfilter.hpp \
    view/poseviewer/undomousecoordinatesmodificationeventfilter.hpp \
    view/settings/settingsloadsavepage.hpp \
    view/splashscreen/splashscreen.hpp \
    view/settings/settingspathspage.hpp \
    view/gallery/loadingiconmodel.hpp \
    view/gallery/galleryobjectmodels.hpp \
    view/misc/displayhelper.hpp \
    view/mainwindow.hpp \
    view/breadcrumb/breadcrumbview.hpp \
    view/gallery/gallery.hpp \
    view/gallery/galleryimagemodel.hpp \
    view/gallery/galleryobjectmodelmodel.hpp \
    view/gallery/iconexpandinglistview.hpp \
    view/rendering/backgroundimagerenderable.hpp \
    view/settings/settingsdialog.hpp \
    view/settings/settingssegmentationcodespage.hpp \
    view/settings/settingsinterfacepage.hpp \
    view/poseviewer/poseviewer.hpp \
    view/poseviewer/poseviewer3dwidget.hpp \
    view/poseeditor/poseeditor.hpp \
    view/poseeditor/poseeditor3dwidget.hpp \
    view/gallery/resizeimagesrunnable.hpp \
    view/rendering/offscreenengine.hpp \
    view/rendering/poserenderable.hpp \
    view/rendering/objectmodelrenderable.hpp \
    view/rendering/texturerendertarget.hpp \
    view/rendering/clickvisualizationmaterial.hpp \
    view/rendering/clickvisualizationrenderable.hpp \
    view/tutorialscreen/tutorialscreen.hpp

SOURCES += \
    $$PWD/poseviewer/renderframegraph.cpp \
    view/poseviewer/mousecoordinatesmodificationeventfilter.cpp \
    view/poseviewer/undomousecoordinatesmodificationeventfilter.cpp \
    view/settings/settingsloadsavepage.cpp \
    view/splashscreen/splashscreen.cpp \
    view/settings/settingspathspage.cpp \
    view/mainwindow.cpp \
    view/breadcrumb/breadcrumbview.cpp \
    view/settings/settingsdialog.cpp \
    view/settings/settingssegmentationcodespage.cpp \
    view/settings/settingsinterfacepage.cpp \
    view/poseviewer/poseviewer.cpp \
    view/poseviewer/poseviewer3dwidget.cpp \
    view/poseeditor/poseeditor.cpp \
    view/poseeditor/poseeditor3dwidget.cpp \
    view/misc/displayhelper.cpp \
    view/gallery/loadingiconmodel.cpp \
    view/gallery/galleryobjectmodels.cpp \
    view/gallery/gallery.cpp \
    view/gallery/galleryimagemodel.cpp \
    view/gallery/galleryobjectmodelmodel.cpp \
    view/gallery/iconexpandinglistview.cpp \
    view/gallery/resizeimagesrunnable.cpp \
    view/rendering/offscreenengine.cpp \
    view/rendering/texturerendertarget.cpp \
    view/rendering/backgroundimagerenderable.cpp \
    view/rendering/poserenderable.cpp \
    view/rendering/objectmodelrenderable.cpp \
    view/rendering/clickvisualizationmaterial.cpp \
    view/rendering/clickvisualizationrenderable.cpp \
    view/tutorialscreen/tutorialscreen.cpp

FORMS += \
    view/settings/settingsloadsavepage.ui \
    view/breadcrumb/breadcrumbview.ui \
    view/settings/settingspathspage.ui \
    view/mainwindow.ui \
    view/gallery/gallery.ui \
    view/settings/settingsdialog.ui \
    view/settings/settingsinterfacepage.ui \
    view/settings/settingssegmentationcodespage.ui \
    view/poseviewer/poseviewer.ui \
    view/poseeditor/poseeditor.ui \
