FROM ubuntu:focal AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update -qq && apt-get -y install \
    gcc   \
    make  \
    cmake \
    checkinstall \
    git   \
    software-properties-common \
    build-essential \
    autoconf \
    libtool \
    moreutils \
    wget \
    unzip \
    yasm \
    pkg-config \
    libswscale-dev \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavformat-dev \
    libpq-dev \
    libopencv-dev \
    python3 \
    python3-dev \
    python3-pybind11 \
&& rm -rf /var/lib/apt/lists/*

RUN add-apt-repository -y ppa:beineri/opt-qt-5.14.2-focal
RUN apt-get update -qq
RUN apt-get -y install qt514-meta-minimal qt5143d qt514gamepad

# This one is for "GL/gl.h", which is required by Qt
#
RUN apt-get -y install mesa-common-dev libglvnd-dev

RUN echo "source /opt/qt514/bin/qt514-env.sh" >> /etc/profile
RUN echo "export CMAKE_PREFIX_PATH=/opt/qt514/" >> /etc/profile
RUN echo "export QMAKESPEC=/opt/qt514/mkspecs/linux-g++" >> /etc/profile

RUN git clone https://github.com/florianblume/6d-pat && \
    cd 6d-pat && \
    mkdir -p build/release && \
    cd build/release && \
    /opt/qt514/bin/qmake ../../6d-pat.pro && \
    make -j6

# To ensure that libpython3.8 will be found
ENV ENV LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PAT
RUN mv /6d-pat/build/release/src/6DPAT /6DPAT

ENTRYPOINT ["/6DPAT"]