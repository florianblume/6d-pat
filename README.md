![The 6D-PAT logo](https://i.imgur.com/P7YhNz5.png "The 6D-PAT logo")

# 6D - Pose Annotation Tool (6D-PAT)

![Build](https://github.com/florianblume/6d-pat/workflows/build/badge.svg) [![Coverage Status](https://coveralls.io/repos/github/florianblume/6d-pat/badge.svg?branch=master)](https://coveralls.io/github/florianblume/6d-pat?branch=master) [![CodeFactor](https://www.codefactor.io/repository/github/florianblume/6d-pat/badge)](https://www.codefactor.io/repository/github/florianblume/6d-pat)  [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/florianblume/6d-pat/issues) 

### What is it?

With 6D-PAT you can create 6D annotations on images for 6D pose estimation, i.e. annotate 2D images with the 3D rotation and 3D translation of 3D models.

### How does it work?

The program allows you to select a folder and view the images contained in it in a gallery. Selecting one of the images will display it at a larger scale to create new 6D pose annotations. The 3D models for those annotations are displayed in a second gallery which also loads the models from a specified folder. In the 3D viewer of the program, you can inspect a selected 3D model, rotate it and use it to create a new pose annotation.

![Imgur](https://i.imgur.com/lwsKfn3.png)

*Objects and images are from the [T-Less Dataset](http://cmp.felk.cvut.cz/t-less/).*

## Getting the program

### Running a release

You can download the latest AppImage from the [releases page](https://github.com/florianblume/6d-pat/releases) which contains everything the program needs to run and should work on the latest Ubuntu out of the box.

### Run the Docker image

Enable X-server display for Docker:

    xhost +local:root
    
Run the Docker image:

    docker run -ti --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix/:/tmp/.X11-unix -v /dev/dri/card0:/dev/dri/card0 florianblume/6dpat /6DPAT

Check out the [getting the program wiki page](https://github.com/florianblume/6d-pat/wiki/1.-Getting-the-Program) for more details.

### Build from source

#### Requirements

|  | OpenGL | Qt | OpenCV
--- | --- | --- | ---
**Version** | *3.1* | *5.14* | *4.5*

You need to have Qt >= 5.14 and OpenCV >= 4.5 installed. You can do so this way for example (if you don't want to manually install Qt and build OpenCV):

    sudo add-apt-repository -y ppa:beineri/opt-qt-5.14.2-focal
    sudo apt-get update -qq
    sudo apt-get -y install qt514-meta-minimal qt5143d qt514gamepad python3 python3-dev python3-pybind11
    sudo apt-get -y install libopencv-dev

Then open the project's main `6d-pat.pro` file in QtCreator and build the project. Everything should compile successfully.

## Setting up the program the first time

Check out the [program setup wiki page](https://github.com/florianblume/6d-pat/wiki/2.-Setting-up-the-Program) to see in detail how to set up the program.

## Recovering poses

To start recovering poses, follow these steps:
1. Select an image
2. Select the corresponding object model
3. Rotate the object model to a similar position as visible in the image
4. Click on the image on a characteristic point of the object
5. Click the same point on the 3D model - the program will show the number of click points at the bottom left
6. Repeat setps 4 - 5 until at least *6* correspondences were created - more correspondences help to make the pose more accurate
7. Click the "Create" button at the bottom of the pose editor
8. You should see the recovered pose on the image
9. You can refine it using the number fields or by dragging or rotating it directly with the mouse after selecting the 3D model
10. After pose refinement, don't forget to press "Save"

More steps and details are on the [wiki page](https://github.com/florianblume/6d-pat/wiki/Recovering-Poses).
