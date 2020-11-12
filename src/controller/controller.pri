INCLUDEPATH += $$PWD

HEADERS  += \
    $$PWD/poserecoveringcontroller.hpp \
    $$PWD/poseseditingcontroller.hpp \
    controller/maincontroller.hpp \
    controller/neuralnetworkcontroller.hpp \
    controller/neuralnetworkrunnable.hpp

SOURCES += \
    $$PWD/poserecoveringcontroller.cpp \
    $$PWD/poseseditingcontroller.cpp \
    controller/maincontroller.cpp \
    controller/neuralnetworkcontroller.cpp \
    controller/neuralnetworkrunnable.cpp
