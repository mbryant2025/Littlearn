#!/bin/bash

# Builds the ESP32 firmware and flashes it to the device.

# TARGET="esp32s3"
# For the esp32-wroom-32d and other non S3 devices, use the following target:
TARGET="esp32"

set -e

DEVICE=$(ls /dev/ttyUSB* | head -n 1)

echo "Found device at $DEVICE"

# Check if 1 is passed as an argument
if [ "$1" == "1" ]; then
    # These only need to be run once, so they are not in the script by default
    . ~/esp/esp-idf/export.sh
    chmod a+rw $DEVICE
fi

idf.py set-target $TARGET
idf.py build
idf.py -p $DEVICE flash