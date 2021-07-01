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
    python3-pybind11 \
&& rm -rf /var/lib/apt/lists/*

# Install OpenCV
#ENV OPENCV_VERSION="4.5.0"
#RUN wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip \
#&& unzip ${OPENCV_VERSION}.zip \
#&& mkdir /opencv-${OPENCV_VERSION}/cmake_binary \
#&& cd /opencv-${OPENCV_VERSION}/cmake_binary \
#&& cmake -DBUILD_TIFF=ON \
#  -DBUILD_opencv_java=OFF \
#  -DWITH_CUDA=OFF \
#  -DBUILD_TESTS=OFF \
#  -DBUILD_PERF_TESTS=OFF \
#  -DCMAKE_BUILD_TYPE=RELEASE \
#  .. \
#&& checkinstall --install=no --default --pkgname=opencv \
#&& cp /opencv-4.5.0/cmake_binary/opencv_*.deb /opencv.deb \
#&& rm /${OPENCV_VERSION}.zip \
#&& rm -r /opencv-${OPENCV_VERSION}

#RUN dpkg -i /opencv.deb

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
    
RUN mv /6d-pat/build/release/src/6DPAT /6DPAT

FROM ubuntu:focal AS runner

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

COPY --from=builder /6DPAT /

RUN apt-get update -qq && apt-get -y install \
    software-properties-common \
    && add-apt-repository -y ppa:beineri/opt-qt-5.14.2-focal \
    && apt-get update -qq \
    && apt-get -y install qt514-meta-minimal qt5143d qt514gamepad libopencv-dev \
