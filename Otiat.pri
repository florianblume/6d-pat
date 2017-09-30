SOURCES += $$PWD/src/main/view/mainwindow.cpp \
    $$PWD/src/main/controller/maincontroller.cpp \
    $$PWD/src/main/model/objectimagecorrespondence.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/model/loadandstorestrategy.cpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.cpp \
    $$PWD/src/main/model/cachingmodelmanager.cpp \
    $$PWD/src/main/model/modelmanager.cpp \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.cpp \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.cpp \
    $$PWD/src/main/view/gallery/gallery.cpp \
    $$PWD/src/main/view/gallery/galleryimagemodel.cpp \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.cpp \
    $$PWD/src/main/view/gallery/iconexpandinglistview.cpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.cpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditorgraphicswidget.cpp \
    $$PWD/src/main/view/aboutdialog/aboutdialog.cpp \
    $$PWD/src/main/view/settings/settingsdialog.cpp \
    $$PWD/src/main/view/settings/settingsgeneralpage.cpp \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.cpp \
    $$PWD/src/main/view/settings/settingsitem.cpp \
    $$PWD/src/main/view/settings/settingsdialogdelegate.cpp \
    $$PWD/src/main/view/correspondenceeditorcontrols/correspondenceeditorcontrols.cpp \
    $$PWD/src/main/view/rendering/objectmodelrenderable.cpp

HEADERS  += \
    $$PWD/src/main/model/cachingmodelmanager.hpp \
    $$PWD/src/main/model/image.hpp \
    $$PWD/src/main/model/loadandstorestrategy.hpp \
    $$PWD/src/main/model/loadandstorestrategylistener.hpp \
    $$PWD/src/main/model/modelmanager.hpp \
    $$PWD/src/main/model/modelmanagerlistener.hpp \
    $$PWD/src/main/model/objectimagecorrespondence.hpp \
    $$PWD/src/main/model/objectmodel.hpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.hpp \
    $$PWD/src/main/controller/maincontroller.hpp \
    $$PWD/src/main/view/mainwindow.hpp \
    $$PWD/src/main/misc/otiathelper.h \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.h \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.h \
    $$PWD/src/main/view/gallery/gallery.h \
    $$PWD/src/main/view/gallery/galleryimagemodel.h \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.h \
    $$PWD/src/main/view/gallery/iconexpandinglistview.h \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.hpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditorgraphicswidget.h \
    $$PWD/src/main/view/aboutdialog/aboutdialog.h \
    $$PWD/src/main/view/settings/settingsdialog.h \
    $$PWD/src/main/view/settings/settingsgeneralpage.h \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.h \
    $$PWD/src/main/view/settings/settingsitem.h \
    $$PWD/src/main/view/settings/settingsdialogdelegate.h \
    src/main/view/correspondenceeditorcontrols/correspondenceeditorcontrols.h \
    $$PWD/src/main/view/rendering/objectmodelrenderable.h

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.ui \
    $$PWD/src/main/view/aboutdialog/aboutdialog.ui \
    $$PWD/src/main/view/settings/settingsdialog.ui \
    $$PWD/src/main/view/settings/settingsgeneralpage.ui \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/src/main/view/correspondenceeditorcontrols/correspondenceeditorcontrols.ui

INCLUDEPATH += \
    $$PWD/src/main/view/breadcrumb \
    $$PWD/src/main/view/navigationcontrols \
    $$PWD/src/main/view/gallery \
    $$PWD/src/main/view/correspondenceeditor \
    $$PWD/src/main \
    $$PWD/src/main/misc \
    $$PWD/src/main/controller \
    $$PWD/src/main/view/settings \

DISTFILES += \
    $$PWD/src/main/view/correspondenceeditor/fragmentShader.fsh \
    $$PWD/src/main/view/correspondenceeditor/vertexShader.vsh
