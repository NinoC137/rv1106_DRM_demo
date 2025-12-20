# ================================
# DRM Cross Compile Environment
# ================================
FROM ubuntu:22.04

LABEL maintainer="drm-cross-dev"
LABEL description="AArch64 DRM cross-compile environment"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC


# --------------------------------
# 1. 基础工具 + ARMHF 交叉编译器
# --------------------------------
RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    file \
    rsync \
    wget \
    ca-certificates \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    vim \
    && rm -rf /var/lib/apt/lists/*

# --------------------------------
# 2. sysroot（挂载）
# --------------------------------
RUN mkdir -p /opt/sysroot

# --------------------------------
# 3. 环境变量
# --------------------------------
ENV SYSROOT=/opt/sysroot
ENV CC=arm-linux-gnueabihf-gcc
ENV CXX=arm-linux-gnueabihf-g++
ENV PKG_CONFIG_SYSROOT_DIR=/opt/sysroot
ENV PKG_CONFIG_PATH=/opt/sysroot/usr/lib/pkgconfig:/opt/sysroot/usr/share/pkgconfig

WORKDIR /work

CMD ["/bin/bash"]
