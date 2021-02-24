# Dockerfile

You can either build the Dockerfile yourself or pull it from florianblume/6dpat. The repository will always have the latest stable release.

    docker run --privileged -e "DISPLAY" -e “QT_X11_NO_MITSHM=1” -v /tmp/.X11-unix:/tmp/.X11-unix -it florianblume/6dpat:latest /6DPAT
