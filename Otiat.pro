#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T16:52:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Otiat
TEMPLATE = app

LIBS += \
       -lboost_system\
       -lboost_filesystem\

SOURCES += src/main/main.cpp \
    src/main/mainwindow.cpp \
    src/main/model/objectimagecorrespondence.cpp \
    src/main/model/image.cpp \
    src/main/model/objectmodel.cpp \
    src/main/model/modelmanager.cpp \
    src/main/maincontroller.cpp \
    src/main/model/loadandstorestrategy.cpp

HEADERS  += \
    src/main/mainwindow.h \
    src/main/objectimagecorrespondence.h \
    src/main/model/objectimagecorrespondence.h \
    src/main/model/image.h \
    src/main/model/objectmodel.h \
    src/main/model/modelmanager.h \
    src/main/model/point.h \
    src/main/maincontroller.h \
    src/main/model/loadandstorestrategy.h

FORMS    += \
    resources/mainwindow.ui
