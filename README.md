# Otiat
Object To Image Annotation Tool - is a tool that allows the user to load a set of images and also a set of 3D 
models and annotate where in the 2D image the 3D object ist placed. After learning the annotations of the user 
the program is able to offer placement suggestions.

## Notes

This project uses the Qt3D rendering framework which comes a long with some peculiarities. For this reason, 
and also for general documentation we want to record them here.

* In contrast to the example offscreen renderer (https://github.com/Sonnentierchen/Qt3D-OfflineRenderer), in this project using setScale on 3D objects causes them to be always drawn from the same angle regardless of the set camera, and also in addition to the correct rendering, i.e. the object is displayed twice, so don't use setScale.

## External Dependencies

Here we will list everything that is needed to successfully run the application.

* Qt5.9
* OpenCV (https://opencv.org/): Clone from git and build. If the path to OpenCV that is stored in the project's file (OtiatSources.pro) differs from the path that your OpenCV installation is located at, adjust it.
* Python 3.6: Adjust the paths to Python in OtiatSources.pro.
