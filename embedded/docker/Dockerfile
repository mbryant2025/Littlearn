FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    vim \
    tree \
    unzip \
    tmux \
    curl \
    python3 \
    python3-pip \
    git \
    valgrind \
    doxygen

# Installing gtest
RUN cd /opt \
    && git clone https://github.com/google/googletest.git \
    && cd googletest \
    && cmake . \
    && make \
    && make install

WORKDIR /root/dev/embedded
