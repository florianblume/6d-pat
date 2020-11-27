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

* OpenGL >= 3.0

## Setup

1. Download the latest release from the releases page.
2. All needed libraries are included in the bundle.
5. You should now be able to run the program directly.

**Check out the wiki pages for other setups!**

# Getting Started

Check out the [wiki page](https://github.com/florianblume/6d-pat/wiki/Setting-up-the-Program) to see in detail how to set up the program.

# Recovering Poses

To start recovering poses, follow these steps:
1. Select an image
2. Select the corresponding object model
3. Rotate the object model to a similar position as visible in the image
4. Click on the image on a characteristic point of the object
5. Click the same point on the 3D model - the program will show the number of click points at the bottom left
6. Repeat setps 4 - 5 until at least *6* correspondences were created - more correspondences help to make the pose more accurate
7. Click the "Create" button at the bottom of the pose editor
8. You should see the recovered pose on the image
9. To refine the pose, select it from the drop down and edit it using the controls
10. After pose refinement, don't forget to press "Save"

![The pose recover process](https://i.imgur.com/JiQqxwv.png "The pose recover process")

More steps and details are on the [wiki page](https://github.com/florianblume/6d-pat/wiki/Recovering-Poses).
