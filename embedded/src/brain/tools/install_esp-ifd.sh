#!/bin/bash

# For Linux machines, to install the ESP-IFD tool, run this script as root.

apt-get install -y \
    wget \
    flex \
    bison \
    gperf   \
    python3-venv \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0

# For Mac, replace the apt-get command with brew install.
# brew install cmake ninja dfu-util

mkdir -p ~/esp \
    && cd ~/esp \
    && git clone --recursive https://github.com/espressif/esp-idf.git \
    && cd ~/esp/esp-idf \
    && ./install.sh esp32s3,esp32 \
    && 
    && . ./export.sh
