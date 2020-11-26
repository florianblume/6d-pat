FROM vookimedlo/ubuntu-qt:5.14_gcc_focal

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
    && rm -rf /var/lib/apt/lists/*

ENV PATH "$PATH:/opt/qt514/bin/qmake"
RUN echo "export PATH=/opt/qt514/bin/qmake:${PATH}" >> /root/.bashrc