# Dockerfile for Environment

This file allows to create an environment that 6D-PAT can be built in - IT'S ONLY THE ENVIRONMENT, NOT THE FULL PROGRAM (for the latter check out the Dockerfile in the _full_ folder). It needs to be run externally though on your local machine. Running 6D-PAT directly in a container can make it feel a bit laggy which is not the case for running it outside the container with the container mounted only.

_The following manual has been taken from [this comment](https://github.com/florianblume/6d-pat/issues/80#issuecomment-609876184)._

1. Pulled the environment image from Docker:

    docker pull florianblume/6dpat-environment

2. Clone the repo locally (outside Docker):

    git clone https://github.com/florianblume/6d-pat

3. For enabling display, execute the following before running the image :

    xhost +local:root

4. Run the image by mounting the 6D-PAT code, NVidia driver folders (for display and rendering using OpenGL) and the images/model folders. The code builds even if you dont mount the NVidia folders but gives an error during runtime :

    docker run --privileged \
     -v {models_path}:/data/models \
     -v {images_path}:/data/images \
     -v /usr/lib/nvidia-410:/usr/lib/nvidia-410 \
     -v /usr/lib32/nvidia-410:/usr/lib32/nvidia-410 \
     --env="DISPLAY"  --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw"  \
     -v {6D-PAT Code}:/data/6d-pat \
     -it florianblume/6dpat:latest

Note, 410 in the above command is my currently active driver version on Ubuntu 16.04. This needs to be changed accordingly for other driver versions

5. Build 6D-PAT:

    cd /data/6d-pat
    mkdir build
    cd build 
    qmake ../6D-PAT.pro
    make -j4

6. Run 6D-PAT:

    export PATH="/usr/lib/nvidia-410/bin":${PATH}
    export LD_LIBRARY_PATH="/usr/lib/nvidia-410:/usr/lib32/nvidia-384":${LD_LIBRARY_PATH}
    ./6D-PAT

