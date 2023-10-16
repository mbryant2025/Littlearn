#!/bin/bash

# This script is used to transfer the code from this repo to the Arduino IDE from when this method is used.

cp ./src/* /Users/michaelbryant/Documents/Arduino/littlearn/
cp ./include/* /Users/michaelbryant/Documents/Arduino/littlearn/

# Remove the main.cpp file from the Arduino IDE folder
rm /Users/michaelbryant/Documents/Arduino/littlearn/main.cpp

# Copy the ino file
cp ./esp32/esp32.ino /Users/michaelbryant/Documents/Arduino/littlearn/littlearn.ino