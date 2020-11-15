INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/gallery/loadingiconmodel.hpp \
    view/gallery/galleryobjectmodels.hpp \
    view/gallery/rendering/offscreenengine.hpp \
    view/gallery/rendering/texturerendertarget.hpp \
    view/misc/displayhelper.hpp \
    view/mainwindow.hpp \
    view/breadcrumb/breadcrumbview.hpp \
    view/gallery/gallery.hpp \
    view/gallery/galleryimagemodel.hpp \
    view/gallery/galleryobjectmodelmodel.hpp \
    view/gallery/iconexpandinglistview.hpp \
    view/navigationcontrols/navigationcontrols.hpp \
    view/rendering/backgroundimagerenderable.hpp \
    view/rendering/objectmodelrenderablematerial.hpp \
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
    view/rendering/objectmodelrenderable.hpp \
    view/rendering/clickvisualizationmaterial.hpp \
    view/rendering/clickvisualizationrenderable.hpp

SOURCES += \
    $$PWD/gallery/loadingiconmodel.cpp \
    view/gallery/galleryobjectmodels.cpp \
    view/mainwindow.cpp \
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
    view/gallery/rendering/offscreenengine.cpp \
    view/gallery/rendering/texturerendertarget.cpp \
    view/rendering/objectmodelrenderable.cpp \
    view/rendering/objectmodelrenderablematerial.cpp \
    view/rendering/clickvisualizationmaterial.cpp \
    view/rendering/clickvisualizationrenderable.cpp

FORMS += \
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
