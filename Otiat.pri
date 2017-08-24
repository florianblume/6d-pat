SOURCES += $$PWD/src/main/mainwindow.cpp \
    $$PWD/src/main/model/objectimagecorrespondence.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/maincontroller.cpp \
    $$PWD/src/main/model/loadandstorestrategy.cpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.cpp \
    $$PWD/src/main/model/cachingmodelmanager.cpp \
    $$PWD/src/main/model/modelmanager.cpp \
    $$PWD/src/main/breadcrumb/breadcrumbview.cpp \
    $$PWD/src/main/navigationcontrols/navigationcontrols.cpp \
    $$PWD/src/main/imagegallery/imagegallery.cpp

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
    $$PWD/src/main/breadcrumb/breadcrumbview.h \
    $$PWD/src/main/navigationcontrols/navigationcontrols.h \
    $$PWD/src/main/imagegallery/imagegallery.h

FORMS    += \
    $$PWD/resources/mainwindow.ui \
    $$PWD/src/main/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/imagegallery/imagegallery.ui
