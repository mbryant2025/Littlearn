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



compiler.cpp.flags=-c -g -Os -std=gnu++11 -fno-exceptions -fno-rtti -Wno-error=narrowing -MMD {compiler.warning_flags} -DF_CPU={build.f_cpu} ...


compiler.cpp.flags=-c -g -Os -std=gnu++11 -fno-exceptions -fno-rtti -fexceptions -Wno-error=narrowing -MMD {compiler.warning_flags} -DF_CPU={build.f_cpu} ...
