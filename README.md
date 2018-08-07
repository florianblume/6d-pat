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

Please be aware that the OpenCV and Assimp libraries are currently loaded from `/usr/lib/x86_64-linux-gnu`, where `apt` installs them. If you want to use a different Qt version than the one installed by `apt` you have to include the Qt libraries manually before including OpenCV and Assimp. The `INCLUDEPATH` AND `LIBS` variables could look like this in this case (leave the rest of the `INCLUDEPATH` of course):
```
INCLUDEPATH += /path/to/qt5.XX/5.XX/gcc_64/include

LIBS += -L/path/to/qt5.XX/5.XX/gcc_64/lib -lQt5Core -lQt5Gui
        -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lassimp
```
Instead of building the program from sources you can also use the pre-built binaries in the folder of the same name. This version expects Qt version 5.9 at apt's default installation location (`/usr/lib/x86_64-linux-gnu`) together with OpenCV and Assimp.

# Getting Started

## Starting the program

After successfully starting the program, you should be presented with an initial view like the following, just without any paths:

![The 6D-PAT UI](https://i.imgur.com/KNeVfOY.png "The 6D-PAT UI")

## Setting up the data paths

Perfect, now open the settings dialog by clicking "Settings" on top. This view should open up (without the paths):

![The 6D-PAT settings dialog](https://i.imgur.com/fHqxbIM.png "The 6D-PAT settings dialog")

Select the folder, where your images reside. The program can load `png` and `jpg/jpeg` files. It is important that the folder containing the images also contains a file called `info.json`, which holds the camera matrix for each individual image. The format of the file as to comply to the following:

```
{
        "image_file_name": {
                                "K": [f_x, 0.0, c_x, 0.0, f_y, c_y, 0.0, 0.0, 1.0]
                           }
}
```

The `image_file_name` is the filename without the full path but including extension, e.g. `0000.jpg`. If you do not have such a file (also in a different format) and you don't know how to create one, please be referred to the (*FlowerPower Neural Network* repository)[https://github.com/Sonnentierchen/flowerpower_nn], which provides a Python function in its utility folder that can create a default camera info file. Be aware, that to recover the poses the real camera matrices should be used.

If you have segmentation images corresponding to your images you can select the respective folder. Leave it like it is otherwise. The program will only load the segmentation images, if their number matches the number of images. If the loading of segmentation images worked correctly, you will be able to switch to viewing it after selecting an image and clicking the toggle at the bottom of the pose viewer (see image further below).

Now, also set the path to the object models that you want to use to recover poses. The program is able to load all 3D formats supported by Assimp.

If you do not have a poses file yet, create an empty JSON file and select it in the settings view under "Poses path".

Depending on the dataset you have, the program should now look a bit like this:

![6D-PAT with loaded images](https://i.imgur.com/dfCrgwa.png "6D-PAT with loaded images")

Click on an image and afterwards on an object model. The program should look like this (of course without the boxes):

![6D-PAT with annotations](https://i.imgur.com/MyHoIhT.png "6D-PAT with annotations")

The images in object models in the picture are from the (T-Less dataset)[http://cmp.felk.cvut.cz/t-less/]. A script to convert the T-Less camera matrix format and ground-truth poses format to the one expected by the program can be found in the utility folder of the FlowerPower repository.
