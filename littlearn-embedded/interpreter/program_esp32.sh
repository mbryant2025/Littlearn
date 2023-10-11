#!/bin/bash

# Install the ESP32 Core for Arduino
arduino-cli core install esp32:esp32

# Install the BLEPeripheral library
arduino-cli lib install BLEPeripheral

# Update the ESP32 Core for Arduino
arduino-cli core update-index
arduino-cli core upgrade esp32:esp32


# Find the port of the connected ESP32
port=$(arduino-cli board list | grep "ttyUSB" | cut -d " " -f 1)
# port="/dev/ttyUSB0"

# Check if a port was found
if [ -z "$port" ]; then
    echo "No Arduino found. Exiting..."
    exit 1
else
    echo "Found Arduino on port: $port"
fi

echo -e "\e[32mCompiling ESP32 sketch...\e[0m"

# Compile the ESP32 sketch
arduino-cli compile --fqbn esp32:esp32:esp32 esp32 --clean

# Check the compilation status
if [ $? -eq 0 ]; then

    echo -e "\e[32mCompilation successful. Uploading to ESP32...\e[0m"
    
    arduino-cli upload -b esp32:esp32:esp32 esp32 -p $port
else
    echo -e "\e[31mCompilation failed.\e[0m"
    exit 1
fi
