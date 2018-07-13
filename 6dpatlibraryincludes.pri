INCLUDEPATH += $$PWD/3dparty/QtAwesome \
    /usr/local/include/opencv \
    /usr/local/include/assimp \
    /home/floretti/anaconda3/envs/pose_estimation/include/python3.5m

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d \
        -L/usr/local/lib -lassimp

LIBS += -L/home/floretti/Qt5.9.5/5.9.5/gcc_64/lib -lQt5Core -lQt5Gui -lQt5Widgets \
        -L/home/floretti/anaconda3/envs/pose_estimation/lib -lpython3.5m
