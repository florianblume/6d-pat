FROM ubuntu:latest AS builder

ARG prefix=/usr/local
ARG binPath=$prefix/bin
ARG libPath=$prefix/lib

# Install dependencies
RUN apt-get update \
    && apt-get install -y \
      libglu1-mesa-dev libxrender1 libxkbcommon-x11-0 \
      g++ \
      gcc \
      build-essential \
      git \
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
      pkg-config \
      xvfb \
      curl \
      libdbus-1-3 \
      libexpat1 \
      libfontconfig1 \
      libfreetype6 \
      libgl1-mesa-glx \
      libglib2.0-0 \
      libx11-6 \
      libx11-xcb1 \
      software-properties-common \
      cmake \ 
      qt5-default \
    && rm -rf /var/lib/apt/lists/*

WORKDIR ${libPath}

# Install OpenCV
RUN git clone https://github.com/opencv/opencv.git && \
    git clone https://github.com/opencv/opencv_contrib.git && \
    cd opencv && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D INSTALL_C_EXAMPLES=ON \
      -D INSTALL_PYTHON_EXAMPLES=ON \
      -D WITH_TBB=ON \
      -D WITH_V4L=ON \
      -D WITH_QT=ON \
      -D WITH_OPENGL=ON \
      -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
      -D BUILD_EXAMPLES=ON .. && \
    make -j4 && \
    make install && \
    make clean && \
    tar -czvf ${libPath}/opencv.tar.gz /usr/local/lib/libopencv* /usr/local/include/opencv4

FROM ubuntu AS runtime

ARG prefix=/usr/local
ARG binPath=$prefix/bin
ARG libPath=$prefix/lib

RUN apt-get update \
    && apt-get install -y \
      libglu1-mesa-dev libxrender1 libxkbcommon-x11-0 \
      g++ \
      gcc \
      build-essential \
      libtbb2 \
      cmake \ 
      qt5-default \
      libassimp-dev

# Copy compiled libraries
COPY --from=builder ${libPath}/opencv.tar.gz ${libPath}/opencv.tar.gz
RUN tar -xvf ${libPath}/opencv.tar.gz -C / && rm ${libPath}/opencv.tar.gz

RUN export LD_LIBRARY_PATH="/lib:/usr/lib:/usr/local/lib"
RUN ldconfig /usr/local/lib
RUN ldconfig