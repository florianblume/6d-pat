SOURCES += $$PWD/src/main/mainwindow.cpp \
    $$PWD/src/main/model/objectimagecorrespondence.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/maincontroller.cpp \
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
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditorgraphicswidget.cpp

HEADERS  += \
    $$PWD/src/main/objectimagecorrespondence.hpp \
    $$PWD/src/main/model/cachingmodelmanager.hpp \
    $$PWD/src/main/model/image.hpp \
    $$PWD/src/main/model/loadandstorestrategy.hpp \
    $$PWD/src/main/model/loadandstorestrategylistener.hpp \
    $$PWD/src/main/model/modelmanager.hpp \
    $$PWD/src/main/model/modelmanagerlistener.hpp \
    $$PWD/src/main/model/objectimagecorrespondence.hpp \
    $$PWD/src/main/model/objectmodel.hpp \
    $$PWD/src/main/model/point.hpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.hpp \
    $$PWD/src/main/maincontroller.hpp \
    $$PWD/src/main/mainwindow.hpp \
    $$PWD/src/main/misc/otiathelper.h \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.h \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.h \
    $$PWD/src/main/view/gallery/gallery.h \
    $$PWD/src/main/view/gallery/galleryimagemodel.h \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.h \
    $$PWD/src/main/view/gallery/iconexpandinglistview.h \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.hpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditorgraphicswidget.h

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.ui

INCLUDEPATH += \
    $$PWD/src/main/view/breadcrumb \
    $$PWD/src/main/view/navigationcontrols \
    $$PWD/src/main/view/gallery \
    $$PWD/src/main/view/correspondenceeditor \
    $$PWD/src/main \
    $$PWD/src/main/misc
