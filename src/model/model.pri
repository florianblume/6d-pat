INCLUDEPATH += $$PWD

HEADERS += \
    model/cachingmodelmanager.hpp \
    model/data.hpp \
    model/image.hpp \
    model/loadandstorestrategy.hpp \
    model/modelmanager.hpp \
    model/objectmodel.hpp \
    model/jsonloadandstorestrategy.hpp \
    model/pose.hpp \
    model/poseeditingmodel.hpp

SOURCES += \
    model/image.cpp \
    model/objectmodel.cpp \
    model/loadandstorestrategy.cpp \
    model/cachingmodelmanager.cpp \
    model/modelmanager.cpp \
    model/jsonloadandstorestrategy.cpp \
    model/pose.cpp
