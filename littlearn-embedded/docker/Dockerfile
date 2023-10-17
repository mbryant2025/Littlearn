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
    valgrind

RUN pip3 install pyserial

RUN cd /opt \
    && git clone https://github.com/google/googletest.git \
    && cd googletest \
    && cmake . \
    && make \
    && make install

RUN cd /opt \
    && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh \
    && mv bin/arduino-cli /bin/arduino-cli \
    && rm -rf bin

WORKDIR /root/dev/interpreter
