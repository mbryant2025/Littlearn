# Running the ./program_esp32.sh Script

Due to the difficulty gaining correct access and mounting, the `program_esp32.sh` script is not run inside Docker. Instead, it is run outside Docker on a Linux machine.


* Before running the script, run:

```bash
sudo apt-get install python3-pip # if pip3 is not installed
pip3 install pyserial # For the internal upload script

# Access to the serial port
sudo usermod -a -G dialout $USER
sudo chmod 666 /dev/ttyUSB0 # or /dev/ttyUSB1, etc.
```

You will need to restart your computer for the permission changes to take effect.



compiler.cpp.flags=-c -g -Os -std=gnu++11 -fno-exceptions -fno-rtti -Wno-error=narrowing -MMD {compiler.warning_flags} -DF_CPU={build.f_cpu} -DLWIP_OPEN_SRC -DARDUINO=10812 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 -DARDUINO_VARIANT=\"esp32\" -DESP32 -DHAVE_CONFIG_H -DPLATFORMIO=50105 -DARDUINO_BOARD=\"ESP32_DEV\" -DARDUINO_ARCH_SAMD -DSTM32F4xx -DNRF5 -DPARTICLE -DPLATFORM_THREADING=0 -DPLATFORM_ID=0 -DPLATFORM_NAME=0 -DUSBD_VID_SPARK=0x2B04 -DUSBD_PID_DFU=0xD00A -DSPARK_PLATFORM -g -w -Os -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -u app_register_globals -u call_user_start_cpu0 -D_GNU_SOURCE -D_REENTRANT -mfix-esp32-psram-cache-issue -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=deprecated-declarations -DWITH_POSIX -DWITH_BOOTLOADER



compiler.cpp.flags=-c -g -Os -std=gnu++11 -fno-exceptions -fno-rtti -fexceptions -Wno-error=narrowing -MMD {compiler.warning_flags} -DF_CPU=compiler.cpp.flags=-c -g -Os -std=gnu++11 -fno-exceptions -fno-rtti -Wno-error=narrowing -MMD {compiler.warning_flags} -DF_CPU={build.f_cpu} -DLWIP_OPEN_SRC -DARDUINO=10812 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 -DARDUINO_VARIANT=\"esp32\" -DESP32 -DHAVE_CONFIG_H -DPLATFORMIO=50105 -DARDUINO_BOARD=\"ESP32_DEV\" -DARDUINO_ARCH_SAMD -DSTM32F4xx -DNRF5 -DPARTICLE -DPLATFORM_THREADING=0 -DPLATFORM_ID=0 -DPLATFORM_NAME=0 -DUSBD_VID_SPARK=0x2B04 -DUSBD_PID_DFU=0xD00A -DSPARK_PLATFORM -g -w -Os -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -u app_register_globals -u call_user_start_cpu0 -D_GNU_SOURCE -D_REENTRANT -mfix-esp32-psram-cache-issue -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=deprecated-declarations -DWITH_POSIX -DWITH_BOOTLOADER -frtti

