![The 6D-PAT logo](https://i.imgur.com/P7YhNz5.png "The 6D-PAT logo")

# 6D - Pose Annotation Tool (6D-PAT)

![Build](https://github.com/florianblume/6d-pat/workflows/build/badge.svg) [![Coverage Status](https://coveralls.io/repos/github/florianblume/6d-pat/badge.svg?branch=master)](https://coveralls.io/github/florianblume/6d-pat?branch=master) [![CodeFactor](https://www.codefactor.io/repository/github/florianblume/6d-pat/badge)](https://www.codefactor.io/repository/github/florianblume/6d-pat)  [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/florianblume/6d-pat/issues) 

### What is it?

With 6D-PAT you can create 6D annotations on images for 6D pose estimation, i.e. annotate 2D images with the 3D rotation and 3D translation of 3D models.

### How does it work?

The program allows you to select a folder and view the images contained in it in a gallery. Selecting one of the images will display it at a larger scale to create new 6D pose annotations. The 3D models for those annotations are displayed in a second gallery which also loads the models from a specified folder. In the 3D viewer of the program, you can inspect a selected 3D model, rotate it and use it to create a new pose annotation.

**Requirements and Setup are only provided for Ubuntu. Given the portability of the used frameworks a setup on Windows, etc. should be possible, as well.**

# Installation

## Requirements

1. Qt >= 5.14
2. OpenCV >= 4.0
4. OpenGL >= 3.0
5. g++ >= 7.3

## Setup

1. To install Qt, open the terminal and execute `sudo apt install qt5-default`.
2. To be able to build or debug the program, install Qt-Creator `sudo apt install qtcreator`.
3. Please build OpenCV yourself, if you haven't done so already. The OpenCV version that is installable under Linux does not output correct poses. You can download OpenCV from [here](https://github.com/opencv/opencv). A tutorial how to make OpenCV can be found [here](https://docs.opencv.org/3.4/d7/d9f/tutorial_linux_install.html).
5. OpenGL should be installed in Ubuntu already.
6. Open Qt-Creator installed in step 2 and open the project. You should now be able to run (i.e. build) the program.

**Check out the wiki pages for other setups!**

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

If you have segmentation images corresponding to your images you can select the respective folder. Leave it like it is otherwise. The program will only load the segmentation images, if their number matches the number of images. If the loading of segmentation images worked correctly, you will be able to switch to viewing it after selecting an image and clicking the toggle at the bottom of the pose viewer (see image further below). This allows you to see misaligned poses better.

Now, also set the path to the object models that you want to use to recover poses. The program is able to load all 3D formats supported by Assimp.

If you do not have a poses file yet, create an empty JSON file and select it in the settings view under "Poses path".

Depending on the dataset you have, the program should now look a bit like this:

![6D-PAT with loaded images](https://i.imgur.com/dfCrgwa.png "6D-PAT with loaded images")

Click on an image and afterwards on an object model. The program should look like this (of course without the boxes):

![6D-PAT with annotations](https://i.imgur.com/MyHoIhT.png "6D-PAT with annotations")

The images in object models in the picture are from the (T-Less dataset)[http://cmp.felk.cvut.cz/t-less/]. A script to convert the T-Less camera matrix format and ground-truth poses format to the one expected by the program can be found in the utility folder of the FlowerPower repository.

The components marked in the picture are the following ones:

**A**: The currently selected path to load images from.
**B**: The gallery showing the loaded images.
**C**: The pose viewer that is able to display a selected image, as well as renders any annoated poses.
**D**: The currently selected path to load object models from.
**E**: The gallery showing the loaded object models.
**F**: The pose editor that is able to display selected object models, which can be rotated using the mouse and moved around using the arrow keys, and allows editing exisiting poses and to recover new ones.

## Additional Setup

When segmentation images exist, you can set the color of the respective tool in the settings view under "Codes". If a segmentation image contains a color that has no object model assigned, all object models will be displayed.

To setup the neural network, open the settings view and click on "Network":

![6D-PAT neural network settings](https://i.imgur.com/J8TPxuK.png "6D-PAT neural network settings")

You can there set the path to you Python interpreter, the training and the inference script. The scrpits are expect to be from the FlowerPower repository. Follow the instructions in that repository to setup a Anaconda environment properly. You have to use the Python interpreter from that environment in order to run the network correctly. As of now, the training script cannot be run yet from within the program. You also have to set the path to the network JSON configuration that you created to run the FlowerPower network in inference mode after training it (again, please refer to the manual in the repository how to set up the network, etc.)

# Recovering Poses

To start recovering poses, follow these steps:
1. Select an image
2. Select the corresponding object model
3. Rotate the object model to a similar position as visible in the image
4. Click on the image on a characteristic point of the object
5. Click the same point on the 3D model - the program will show the number of click points at the bottom left
6. Repeat setps 4 - 5 until at least 4 correspondences were created - more correspondences help to make the pose more accurate
7. Click the "Create" button at the bottom of the pose editor
8. You should see the recovered pose on the image
9. To refine the pose, select it from the drop down and edit it using the controls
10. After pose refinement, don't forget to press "Save"

![The pose recover process](https://i.imgur.com/JiQqxwv.png "The pose recover process")

You can always abort the creation process from the edit menu. To use the neural network on the current image, press the "Predict" button (not visible in the image). To run network inference on multiple images select "Network" from the menu and select the images to run inference on. The program automatically writes the images into the image list defined in the config and sets the proper poses file (the one that you selected in the settings).

You can also remove poses using the "Remove" button and adjust the transparency of the objects on the image using the slider labled "Transparency". This allows you to see the image behind overlapping object models.

# Hurray! You're good to go and can now annotate millions of images!

**Some more screenshots of the program:**

The program showing the T-Less ground-truth poses:
![The program showing the T-Less ground-truth poses](https://i.imgur.com/U7UuETx.png "The program showing the T-Less ground-truth poses")

The program showing the corresponding segmentation mask, which were rendered manually beforehand using the ground-truth files of T-Less. The toggle is highlighted with the green rectangle:
![TThe program showing the corresponding segmentation mask, which were rendered manually beforehand using the ground-truth files of T-Less](https://i.imgur.com/lWNDisG.png "The program showing the corresponding segmentation mask, which were rendered manually beforehand using the ground-truth files of T-Less. The toggle is highlighted with the green rectangle.")
