![The 6D-PAT logo](https://i.imgur.com/P7YhNz5.png "The 6D-PAT logo")

# 6D - Pose Annotation Tool (6D-PAT)

6D-PAT is a program that enables users to recover poses of 3D object models on images. It allows to view images of a folder in a gallery and select those images for annotation. Likewise, it can display 3D models contained in a folder in a gallery and offers a 3D viewer. The displayed image an object model can then be used to annotate the pose of the object on the image. It also offers basic functionality of using a neural network written in Python to predict poses for images.

**Requirements and Setup are only provided for Ubuntu. Given the portability of the used frameworks a setup on Windows, etc. should be possible, as well.**

# Requirements

1. Qt >= 5.6
2. OpenCV >= 3.2
3. Assimp >= 4.1
4. OpenGL >= 3.0

# Setup

1. To install Qt, open the terminal and execute `sudo apt install qt5-default`.
2. To install OpenCV, open the terminal and execute `sudo apt install libopencv-dev`.
3. To install Assimp. open the terminal and execute `sudo apt install libassimp-dev`.
4. OpenGL should be installed in Ubuntu alread.

You can also build 1 - 3 from the respective sources. Please refer to the documentations how to achieve this. If you want to use your custom built OpenCV and Assimp you have to adjust the `INCLUDEPATH` and `LIBS` variable in 6dpatsources.pri. For example, change the path to the OpenCV include files from `/usr/include/opencv` to `/usr/local/include/opencv`.

Please be aware that the OpenCV and Assimp libraries are currently loaded from `/usr/lib/x86_64-linux-gnu`, where `apt` installs them. If you want to use a different Qt version than the one installed by `apt` you have to include the Qt libraries manually before including OpenCV and Assimp. The `INCLUDEPATH` AND `LIBS` variables could look like this in this case:
```
INCLUDEPATH += /path/to/qt5.XX/5.XX/gcc_64/include

LIBS += -L/path/to/qt5.XX/5.XX/gcc_64/lib -lQt5Core -lQt5Gui
        -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lassimp
```
