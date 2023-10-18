#!/bin/bash

# This script is used to transfer the code from this repo to the Arduino IDE from when this method is used.
# Feel free to modify this script to your needs.

ARDUINO_PATH="/Users/michaelbryant/Documents/Arduino/littlearn"

cp ./src/* "$ARDUINO_PATH"
cp ./include/* "$ARDUINO_PATH"

# Remove the main.cpp file from the Arduino IDE folder
rm "$ARDUINO_PATH/main.cpp"

# Copy the ino file
cp ./esp32/esp32.ino "$ARDUINO_PATH/littlearn.ino"
